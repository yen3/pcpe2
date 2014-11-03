#if defined(__GTEST_PCPE__)
#include "hash_table.h"
#include "pcpe_util.h"

#include <map>
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

#include <gtest/gtest.h>

namespace pcpe{

/*****************************************************************************/
// Typedef  
/*****************************************************************************/
class CommonSubseqTask;

typedef std::string Seq;
typedef std::vector<Seq> SeqList;

typedef std::size_t SeqIndex;
typedef std::size_t SubstrIndex;

/* 
 * key:    hash value
 * value:  <substring (6 chars), <sequence index, substring location index> >
 * */
typedef std::vector<
    std::map<Seq, std::vector<std::pair<SeqIndex, SubstrIndex> > > > HashTable;

typedef std::vector<CommonSubseqTask> CommonSubseqTaskList;

/*****************************************************************************/
// Function protypes 
/*****************************************************************************/
const std::size_t HASH_TABLE_SIZE= 100003;
const std::size_t SUBSTRING_SIZE = 6;

extern std::atomic_uint cstl_index;

extern void read_sequence_list(SeqList& seq, const char* filename);

extern std::size_t hash_value(const char* s);

extern void add_substring_to_hashtable(const std::size_t seq_index,
                                       const std::string& s,
                                       HashTable& ht);

extern std::shared_ptr<HashTable>
create_hash_table(const std::string& filename);

extern std::ostream& operator<<(std::ostream& os,
                                const CommonSubseqTask& cst);

extern void create_compare_hashtable_task_list(std::vector<Filename>& out_fn_list,
                                               CommonSubseqTaskList& cstl,
                                               int task_size,
                                               int hash_table_size,
                                               const Filename& out_fn_prefix);
extern void compare_hashtable_part(const CommonSubseqTaskList& cstl,
                                   const HashTable& x,
                                   const HashTable& y,
                                   int thread_num);

extern std::shared_ptr<std::vector<Filename>>
common_subseq_files(const HashTable& x,
                    const HashTable& y,
                    const Filename& out_fn_prefix="sub_hash");

extern std::shared_ptr<std::vector<Filename> >
common_subseq(Filename fn_seq_a,
              Filename fn_seq_b);


/*****************************************************************************/
// Test functions 
/*****************************************************************************/
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

}
#endif
