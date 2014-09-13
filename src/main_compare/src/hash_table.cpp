#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <memory>
#include <future>
#include <atomic>

#if defined(__GTEST_PCPE__)
#include <map>
#include <gtest/gtest.h>
#endif

#include "hash_table.h"

namespace pcpe{

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
const std::size_t TASK_SIZE = 100;
const std::size_t HASH_TABLE_SIZE= 100003;
const std::size_t SUBSTRING_SIZE = 6;

std::atomic_uint cstl_index(0);

/*****************************************************************************/
// Typedef  
/*****************************************************************************/
typedef std::string Seq;
typedef std::vector<Seq> SeqList;

typedef std::size_t SeqIndex;
typedef std::size_t SubstrIndex;

/* 
 * key:    hash value
 * value:  <substring (6 chars), <sequence index, substring location index> >
 * */
typedef std::vector<std::map<Seq, std::vector<std::pair<SeqIndex, SubstrIndex> > > > HashTable;


/*****************************************************************************/
// Functions 
/*****************************************************************************/

/**
 * @brief read sequence list from formated fasta file 
 *
 * @param[out] seq      sequence list
 * @param[in]  filename input filename
 *
 */
void read_sequence_list(SeqList& seq, const char* filename)
{
    std::ifstream in_file(filename, std::ifstream::in);

    std::size_t read_size=0, str_read_size=0;
    std::string s;
    in_file >> str_read_size;
    for(std::size_t i=0; i<str_read_size; i++){
        in_file >> read_size >> s;
        seq.push_back(std::move(s));
    }

    in_file.close();
    in_file.clear();
}

inline std::size_t hash_value(const char* s)
{
    return (s[0]-'A')*11881376 + (s[1]-'A')*456976 + (s[2]-'A')*17576 + (s[3]-'A')*676 + (s[4]-'A')*26 + (s[5]-'A')*1;
}


void add_substring_to_hashtable(const std::size_t seq_index, const std::string& s, HashTable& ht)
{
    if(s.size() < SUBSTRING_SIZE) return;

    for(std::size_t substr_index = 0;
        substr_index < s.size()-SUBSTRING_SIZE;
        ++substr_index){
#if defined(__DEBUG__)
        try{
            ht[hash_value(s.c_str()+ substr_index) % ht.size()]
                [s.substr(substr_index, SUBSTRING_SIZE)].push_back(std::make_pair(seq_index, substr_index));
        }catch(const std::out_of_range& oor){
            std::cerr << "Error " << substr_index << " " << s << " " << oor.what() << std::endl;
            exit(1);
        }
#else 
        ht[hash_value(s.c_str()+ substr_index) % ht.size()]
            [std::move(s.substr(substr_index, SUBSTRING_SIZE))].push_back(
                    std::move(std::make_pair(seq_index, substr_index)));
#endif /*  __DEBUG__ */
    }
}

/**
 * @brief create hash table from file 
 *
 * @param[out] ht       hash table
 * @param[in]  filename input filename
 *
 * @return     a shared ptr to hash table
 */
std::shared_ptr<HashTable> create_hash_table(const std::string& filename)
{
    std::shared_ptr<HashTable> ht(new HashTable(HASH_TABLE_SIZE));

    // read sequence list from file
    SeqList seq_list;
    read_sequence_list(seq_list, filename.c_str());

    // add substrings information to hashtable
    for(std::size_t seq_index=0; seq_index< seq_list.size(); ++seq_index){
        add_substring_to_hashtable(seq_index, seq_list[seq_index], *ht);
    }

    return ht;
}

inline void output_to_file(ComSubseqFileWriter& out_file,
                           const std::string& mcs,
                           const std::vector<std::pair<SeqIndex, SubstrIndex>>& lx, 
                           const std::vector<std::pair<SeqIndex, SubstrIndex>>& ly)
{
    for(const auto& x : lx){
        for(const auto& y: ly){
            out_file.writeSeq(ComSubseq(x.first, y.first, x.second, y.second, 6));
        }
    }
}


class CommonSubseqTask{
    friend std::ostream& operator<<(std::ostream& os, const CommonSubseqTask& cst);
public:
    CommonSubseqTask(std::size_t begin_index,
                     std::size_t end_index,
                     const Filename& filename):begin(begin_index),
                                               end(end_index),
                                               fn(filename)
    {
    };

    const std::size_t begin;
    const std::size_t end;
    const Filename fn;
};

typedef std::vector<CommonSubseqTask> CommonSubseqTaskList;

std::ostream& operator<<(std::ostream& os, const CommonSubseqTask& cst){
    os << cst.fn << " " << cst.begin << " " << cst.end;
    return os;
};


void 
create_compare_hashtable_task_list(std::vector<Filename>& out_fn_list,
                                   CommonSubseqTaskList& cstl,
                                   int task_size,
                                   int hash_table_size,
                                   const Filename& out_fn_prefix)
{
    out_fn_list = std::move(std::vector<Filename>(TASK_SIZE));


    for(std::size_t fn_index = 0; fn_index < out_fn_list.size(); fn_index++){
        std::ostringstream oss;
        oss << out_fn_prefix << "_" << fn_index; 
        out_fn_list[fn_index] = oss.str();
    }

    // Each task has begin index to end index for the hash table
    for(std::size_t t_index = 0,
                    index_size = HASH_TABLE_SIZE/ TASK_SIZE;
        t_index < TASK_SIZE;
        ++t_index){
        cstl.push_back(std::move(CommonSubseqTask(t_index*index_size,
                   (t_index == TASK_SIZE - 1)? HASH_TABLE_SIZE:
                                               (t_index+1)*index_size,
                   out_fn_list[t_index])));
    }
}


void compare_hashtable_part(const CommonSubseqTaskList& cstl, const HashTable& x, const HashTable& y, int thread_num)
{
    std::size_t local_cstl_index;
    while(1){
        local_cstl_index = cstl_index.fetch_add(1);
        if(local_cstl_index >= cstl.size()){
            break;
        }

        ComSubseqFileWriter outfile(cstl[local_cstl_index].fn);
        for(std::size_t midx = cstl[local_cstl_index].begin;
            midx < cstl[local_cstl_index].end;
            ++midx){
            for(auto hx = x[midx].begin(), hy = y[midx].begin();
                hx != x[midx].end() && hy != y[midx].end();)
            {
                if(hx->first == hy->first){
                    output_to_file(outfile, hx->first, hx->second, hy->second);
                    ++hx;
                    ++hy;
                }
                else if(hx->first > hy->first)  ++hy;
                else   ++hx;
            }
        }
        outfile.close();
    }
#if 0
    std::size_t local_cstl_index;
    while(1){
        local_cstl_index = cstl_index.fetch_add(1);
        if(local_cstl_index > cstl.size()){
            break;
        }

        ComSubseqFileWriter out_file(cstl[local_cstl_index].fn);
        for(std::size_t midx = cstl[local_cstl_index].begin;
            midx < cstl[local_cstl_index].end;
            ++midx){
            for(auto hx = x[midx].begin(), hy = y[midx].begin();
                hx != x[midx].end() && hy != y[midx].end();)
            {
                if(hx->first == hy->first){
                    output_to_file(out_file, hx->first, hx->second, hy->second);
                    ++hx;
                    ++hy;
                }
                else if(hx->first > hy->first)  ++hy;
                else   ++hx;
            }
        }
        out_file.close();
    }
#endif
}


std::shared_ptr<std::vector<Filename>>
common_subseq_files(const HashTable& x,
                    const HashTable& y,
                    const Filename& out_fn_prefix="sub_hash")
{
    if(x.size() != y.size()){
        return nullptr;
    }

    // Get Task list and output filename list
    std::shared_ptr<std::vector<Filename>> out_fn_list(new std::vector<Filename>());
    CommonSubseqTaskList cstl;
    create_compare_hashtable_task_list(*out_fn_list, cstl, TASK_SIZE, x.size(), out_fn_prefix);

    // start to run the all tasks
    std::vector<std::thread> tasks(std::thread::hardware_concurrency());
    std::size_t thread_num = 0;
#if __DEBUG__
    std::cout << "craete " <<  tasks.size() << " thread" << std::endl;
#endif
    for(auto& task : tasks){
        task = std::thread(compare_hashtable_part, cstl, x, y, thread_num);
        thread_num++;
    }
    for(auto& task : tasks){
        task.join();
    }
    
    return out_fn_list;
}


/**
  *  List all common subsequences whose length are 6.
  *
  *  @param[in]  fn_seq_a   filename of sequence a
  *  @param[in]  fn_seq_b   filename of sequence b
  *
  *  @return  a share pointer to a filename list. Each file coantains part of
  *           common subsequences, but not sorted. If you want to use these
  *           informatinos, you have to sort these files and merge sort them to
  *           locate the maximum subsequences.
  * 
  */
std::shared_ptr<std::vector<Filename> >
common_subseq(Filename fn_seq_a,
              Filename fn_seq_b)
{

#if defined(__DEBUG__)
    std::cout << fn_seq_a << std::endl << fn_seq_b << std::endl;
#endif /* __DEBUG__ */

    auto create_hta = std::async(create_hash_table, fn_seq_a);
    auto create_htb = std::async(create_hash_table, fn_seq_b);

    std::shared_ptr<HashTable> phta = create_hta.get();
    std::shared_ptr<HashTable> phtb = create_htb.get();

    auto out_fn_list = common_subseq_files(*phta, *phtb);
#if defined(__DEBUG__)
    std::cout << __FILE__ << ": " << __LINE__ << ": " << cstl_index << std::endl;  
#endif
    return out_fn_list;
}


/*****************************************************************************/
// Test Case 
/*****************************************************************************/
#if defined(__GTEST_PCPE__)

TEST(hash_table, hash_value){
    EXPECT_EQ(0, hash_value("AAAAAA"));
    EXPECT_EQ(1, hash_value("AAAAAB"));
    EXPECT_EQ(27, hash_value("AAAABB"));
    EXPECT_EQ(703, hash_value("AAABBB"));
}


TEST(hash_table, add_substring_to_hashtable){
    std::string s("ABCDABCDABCD");
    HashTable ht(HASH_TABLE_SIZE);

    std::map<std::string, int> sub_count;
    for(std::size_t i=0; i<s.size()-SUBSTRING_SIZE; i++){
        sub_count[s.substr(i, SUBSTRING_SIZE)] ++;
    }

    add_substring_to_hashtable(1, s, ht);

    EXPECT_EQ(HASH_TABLE_SIZE, ht.size());
    for(auto iter=sub_count.begin(); iter != sub_count.end(); ++iter){
        const std::string& substring = iter->first;
        const int substring_count = iter->second;
        const std::size_t hash_index = hash_value(substring.c_str()) % ht.size();

        EXPECT_EQ(substring_count, ht[hash_index][substring].size());
    }

}

#endif

}
