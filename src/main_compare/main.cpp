#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <memory>
#include <future>

const std::size_t HASH_TABLE_SIZE= 100003;
const std::size_t SUBSTRING_SIZE = 6;

typedef std::vector<std::string> SeqList;

/* 
 * key:    hash value
 * value:  <substring (6 chars), <sequence index, substring location index> >
 * */
typedef std::vector<std::map<std::string, std::vector<std::pair<int,int> > > > HashTable;


/**
 * @brief read sequence list from formated fasta file 
 *
 * @param[out] seq      sequence list
 * @param[in]  filename input filename
 */
void read_sequence_list(SeqList& seq, const char* filename)
{
    std::ifstream in_file(filename, std::ifstream::in);

    std::size_t read_size=0, str_read_size=0;
    std::string s;
    in_file >> str_read_size;
    for(std::size_t i=0; i<str_read_size; i++){
        in_file >> read_size >> s;
        seq.push_back(s);
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
            [s.substr(substr_index, SUBSTRING_SIZE)].push_back(
                    std::make_pair(seq_index, substr_index));
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

inline void output_to_file(std::ofstream& out_file,
                           const std::string& mcs,
                           const std::vector<std::pair<int, int>>& lx, 
                           const std::vector<std::pair<int, int>>& ly)
{
    for_each(lx.begin(), lx.end(),
        [&out_file, &mcs, &ly](const std::pair<int, int>& x){
            for_each(ly.begin(), ly.end(), 
                [&out_file, &mcs, &x](const std::pair<int, int>& y){
                        out_file << x.first << " " << y.first << " "
                                 << x.second << " " << y.second << " " 
                                 << mcs << std::endl;
            });
    });
}

int compare_hash_table(const std::string& out_fn, const HashTable& x, const HashTable& y)
{
    if(x.size() != y.size()){
        return -1;
    }
#if defined(__DEBUG__)
    std::cout << "start to compare hash table" << std::endl;
#endif /* __DEBUG__ */

    std::ofstream out_file(out_fn, std::ofstream::out);

    for(std::size_t m_index = 0; m_index < x.size(); ++m_index){
        for(auto hx = x[m_index].begin(), hy = y[m_index].begin();
                 hx != x[m_index].end() && hy != y[m_index].end();)
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

    out_file.clear();
    out_file.close();

    return 0;
}

int main(int argc, char const* argv[])
{

    /**
     *   argv[1]: input sequence a
     *   argv[2]: input sequence b
     *   argv[3]: output file
     * */
    std::string input_fn_a;
    std::string input_fn_b;
    std::string output_fn;

    if(argc == 4){
        input_fn_a = argv[1];
        input_fn_b = argv[2];
        output_fn = argv[3];
    }

#if defined(__DEBUG__)
    std::cout << input_fn_a << std::endl << input_fn_b << std::endl << output_fn << std::endl;
#endif /* __DEBUG__ */

    auto create_hta = std::async(create_hash_table, input_fn_a);
    auto create_htb = std::async(create_hash_table, input_fn_b);

    std::shared_ptr<HashTable> phta = create_hta.get();
    std::shared_ptr<HashTable> phtb = create_htb.get();

    compare_hash_table(output_fn, *phta, *phtb);

    return 0;
}

