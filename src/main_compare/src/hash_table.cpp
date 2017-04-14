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
#include <cassert>

#if defined(__GTEST_PCPE__)
#include <map>
#include <gtest/gtest.h>
#endif

#include "hash_table.h"

#if defined(__DEBUG__)
#define DEBUG_PRINT std::cout << __FILE__ << ": " << __LINE__ << ": "
#endif

namespace pcpe {

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
std::atomic_uint cstl_index(0);

/*****************************************************************************/
// Functions - Build hash table
/*****************************************************************************/

/**
 * @brief read sequence list from formated fasta file
 *
 * @param[out] seq      sequence list
 * @param[in]  filename input filename
 *
 */
void read_sequence_list(SeqList& seq, const char* filename) {
    std::ifstream in_file(filename, std::ifstream::in);

    std::size_t read_size = 0, str_read_size = 0;
    std::string s;
    in_file >> str_read_size;
    for (std::size_t i = 0; i < str_read_size; i++) {
        in_file >> read_size >> s;
        seq.push_back(std::move(s));
#if __DEBUG__
        assert(seq[seq.size() - 1].size() == read_size);
#endif
    }

#if __DEBUG__
    assert(str_read_size == seq.size());
#endif

    in_file.close();
    in_file.clear();
}

std::size_t hash_value(const char* s) {
    return (s[0] - 'A') * 11881376 + (s[1] - 'A') * 456976 +
           (s[2] - 'A') * 17576 + (s[3] - 'A') * 676 + (s[4] - 'A') * 26 +
           (s[5] - 'A') * 1;
}

void add_substring_to_hashtable(const std::size_t seq_index,
                                const std::string& s, HashTable& ht) {
    if (s.size() < SUBSTRING_SIZE) return;

    for (std::size_t substr_index = 0;
         substr_index <= s.size() - SUBSTRING_SIZE; ++substr_index) {
#if defined(__DEBUG__)
        try {
            ht[hash_value(s.c_str() + substr_index) %
               ht.size()][s.substr(substr_index, SUBSTRING_SIZE)]
                .push_back(std::make_pair(seq_index, substr_index));
        } catch (const std::out_of_range& oor) {
            std::cerr << "Error " << substr_index << " " << s << " "
                      << oor.what() << std::endl;
            exit(1);
        }
#else
        ht[hash_value(s.c_str() + substr_index) %
           ht.size()][std::move(s.substr(substr_index, SUBSTRING_SIZE))]
            .push_back(std::move(std::make_pair(seq_index, substr_index)));
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
std::shared_ptr<HashTable> create_hash_table(const std::string& filename) {
    std::shared_ptr<HashTable> ht(new HashTable(HASH_TABLE_SIZE));

    // read sequence list from file
    SeqList seq_list;
    read_sequence_list(seq_list, filename.c_str());

    // add substrings information to hashtable
    for (std::size_t seq_index = 0; seq_index < seq_list.size(); ++seq_index) {
        add_substring_to_hashtable(seq_index, seq_list[seq_index], *ht);
    }

    return ht;
}

/*****************************************************************************/
// Functions - Compare hash table
/*****************************************************************************/

inline void output_to_file(
    ComSubseqFileWriter& out_file, const std::string& mcs,
    const std::vector<std::pair<SeqIndex, SubstrIndex>>& lx,
    const std::vector<std::pair<SeqIndex, SubstrIndex>>& ly) {
    for (const auto& x : lx) {
        for (const auto& y : ly) {
            out_file.writeSeq(ComSubseq(x.first, y.first, x.second, y.second,
                                        SUBSTRING_SIZE));
#if 0
            std::cout << "(" << x.first << ", " << y.first << ", " << x.second << ", " << y.second << ")" << std::endl;
#endif
        }
    }
}

void create_compare_hashtable_task_list(std::vector<Filename>& out_fn_list,
                                        CommonSubseqTaskList& cstl,
                                        const std::size_t task_size,
                                        const std::size_t hash_table_size,
                                        const Filename& out_fn_prefix) {
    out_fn_list = std::move(std::vector<Filename>(TASK_SIZE));

    for (std::size_t fn_index = 0; fn_index < out_fn_list.size(); fn_index++) {
        std::ostringstream oss;
        oss << out_fn_prefix << "_" << fn_index;
        out_fn_list[fn_index] = oss.str();
    }

    // Each task has begin index to end index for the hash table
    for (std::size_t t_index = 0, index_size = HASH_TABLE_SIZE / TASK_SIZE;
         t_index < TASK_SIZE; ++t_index) {
        cstl.push_back(std::move(CommonSubseqTask(
            t_index * index_size,
            (t_index == TASK_SIZE - 1) ? HASH_TABLE_SIZE
                                       : (t_index + 1) * index_size,
            out_fn_list[t_index])));
    }
}

void compare_hashtable_part(const CommonSubseqTask& cst, const HashTable& x,
                            const HashTable& y) {
    ComSubseqFileWriter outfile(cst.fn);

    for (std::size_t midx = cst.begin; midx < cst.end; ++midx) {
        for (auto hx = x[midx].begin(); hx != x[midx].end(); ++hx) {
            for (auto hy = y[midx].begin(); hy != y[midx].end(); ++hy) {
                if (hx->first == hy->first) {
                    output_to_file(outfile, hx->first, hx->second, hy->second);
                }
            }
        }
    }
    outfile.close();
}

void compare_hashtable_task(const CommonSubseqTaskList& cstl,
                            const HashTable& x, const HashTable& y) {
    std::size_t local_cstl_index;
    while (1) {
        local_cstl_index = cstl_index.fetch_add(1);
        if (local_cstl_index >= cstl.size()) {
            break;
        }
#if defined(__DEBUG__) && !defined(__GTEST_PCPE__)
        DEBUG_PRINT << local_cstl_index << std::endl;
#endif

        compare_hashtable_part(cstl[local_cstl_index], x, y);
    }
}

std::shared_ptr<std::vector<Filename>> common_subseq_files(
    const HashTable& x, const HashTable& y,
    const Filename& out_fn_prefix = "sub_hash") {

    if (x.size() != y.size()) {
        return nullptr;
    }

    // Get Task list and output filename list
    std::shared_ptr<std::vector<Filename>> out_fn_list(
        new std::vector<Filename>());
    CommonSubseqTaskList cstl;
    create_compare_hashtable_task_list(*out_fn_list, cstl, TASK_SIZE, x.size(),
                                       out_fn_prefix);

    // start to run the all tasks
    std::vector<std::thread> tasks(std::thread::hardware_concurrency());
#if defined(__DEBUG__) && !defined(__GTEST_PCPE__)
    DEBUG_PRINT << "create " << tasks.size() << " thread" << std::endl;
#endif
    for (auto& task : tasks) {
        task = std::thread(compare_hashtable_task, cstl, x, y);
    }

    for (auto& task : tasks) {
        task.join();
    }

    return out_fn_list;
}

void
common_subseq(const Filename& fn_seq_a,
              const Filename& fn_seq_b,
              FilenameList& output_fn_list,
              const Filename& temp_file_prefix)

{
#if defined(__DEBUG__)
    DEBUG_PRINT << fn_seq_a << std::endl << fn_seq_b << std::endl;
#endif /* __DEBUG__ */

    auto create_hta = std::async(create_hash_table, fn_seq_a);
    auto create_htb = std::async(create_hash_table, fn_seq_b);

    std::shared_ptr<HashTable> phta = create_hta.get();
    std::shared_ptr<HashTable> phtb = create_htb.get();

#if defined(__DEBUG__)
    DEBUG_PRINT << "create hash table done" << std::endl;
#endif

    auto output_fns = common_subseq_files(*phta, *phtb, temp_file_prefix);

#if defined(__DEBUG__)
    DEBUG_PRINT << cstl_index << std::endl;
#endif

    output_fn_list = *output_fns;
}

}
