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

namespace pcpe {

/*****************************************************************************/
// Function protypes
/*****************************************************************************/
extern void read_sequence_list(SeqList& seq, const char* filename);

extern std::size_t hash_value(const char* s);

extern void add_substring_to_hashtable(const std::size_t seq_index,
                                       const std::string& s, HashTable& ht);

extern std::shared_ptr<HashTable> create_hash_table(
    const std::string& filename);

extern std::ostream& operator<<(std::ostream& os, const CommonSubseqTask& cst);

extern void create_compare_hashtable_task_list(
    std::vector<Filename>& out_fn_list, CommonSubseqTaskList& cstl,
    const std::size_t task_size, const std::size_t hash_table_size,
    const Filename& out_fn_prefix);
extern void compare_hashtable_part(const CommonSubseqTask& cst,
                                   const HashTable& x, const HashTable& y);

extern void compare_hashtable_task(const CommonSubseqTaskList& cstl,
                                   const HashTable& x, const HashTable& y);

extern std::shared_ptr<std::vector<Filename>> common_subseq_files(
    const HashTable& x, const HashTable& y,
    const Filename& out_fn_prefix = "sub_hash");

extern std::shared_ptr<std::vector<Filename>> common_subseq(Filename fn_seq_a,
                                                            Filename fn_seq_b);

/*****************************************************************************/
// Test functions
/*****************************************************************************/
TEST(hash_table, hash_value) {
    EXPECT_EQ(0, hash_value("AAAAAA"));
    EXPECT_EQ(1, hash_value("AAAAAB"));
    EXPECT_EQ(27, hash_value("AAAABB"));
    EXPECT_EQ(703, hash_value("AAABBB"));
}

TEST(hash_table, read_sequence_list) {
    std::string filename("./testdata/test_seq1.txt");

    SeqList seq_list;
    read_sequence_list(seq_list, filename.c_str());
    EXPECT_EQ(seq_list.size(), 3);
    EXPECT_EQ(seq_list[0], "ABCDEFG");
    EXPECT_EQ(seq_list[1], "ABCDEFGH");
    EXPECT_EQ(seq_list[2], "ABCDEFGHI");
}

TEST(hash_table, read_sequence_list_2) {
    std::string filename("./testdata/test_seq2.txt");

    SeqList seq_list;
    read_sequence_list(seq_list, filename.c_str());
    EXPECT_EQ(seq_list.size(), 2);
    EXPECT_EQ(seq_list[0], "BCDEFG");
    EXPECT_EQ(seq_list[1], "CDEFGHI");
}

TEST(hash_table, add_substring_to_hashtable) {
    std::string s("ABCDEFGHI");
    HashTable ht(HASH_TABLE_SIZE);

    add_substring_to_hashtable(1, s, ht);

    EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()].size(), 1);

    EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()]["ABCDEF"].size(), 1);
    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"].size(), 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"].size(), 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"].size(), 1);

    EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()]["ABCDEF"][0].first, 1);
    EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()]["ABCDEF"][0].second, 0);

    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"][0].first, 1);
    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"][0].second, 1);

    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][0].first, 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][0].second, 2);

    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"][0].first, 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"][0].second, 3);
}

TEST(hash_table, test_create_hash_table) {
    std::string filename("./testdata/test_seq1.txt");

    std::shared_ptr<HashTable> pht = create_hash_table(filename);

    HashTable& ht = *pht;

    EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()].size(), 1);

    EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()]["ABCDEF"].size(), 3);
    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"].size(), 3);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"].size(), 2);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"].size(), 1);

    for (std::size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()]["ABCDEF"][i].first, i);
        EXPECT_EQ(ht[hash_value("ABCDEF") % ht.size()]["ABCDEF"][i].second, 0);
    }

    for (std::size_t i = 0; i < 3; ++i) {
        EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"][i].first, i);
        EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"][i].second, 1);
    }

    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][0].first, 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][0].second, 2);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][1].first, 2);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][1].second, 2);

    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"][0].first, 2);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"][0].second, 3);
}

TEST(hash_table, test_create_hash_table_2) {
    std::string filename("./testdata/test_seq2.txt");

    std::shared_ptr<HashTable> pht = create_hash_table(filename);

    HashTable& ht = *pht;

    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()].size(), 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()].size(), 1);

    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"].size(), 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"].size(), 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"].size(), 1);

    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"][0].first, 0);
    EXPECT_EQ(ht[hash_value("BCDEFG") % ht.size()]["BCDEFG"][0].second, 0);

    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][0].first, 1);
    EXPECT_EQ(ht[hash_value("CDEFGH") % ht.size()]["CDEFGH"][0].second, 0);

    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"][0].first, 1);
    EXPECT_EQ(ht[hash_value("DEFGHI") % ht.size()]["DEFGHI"][0].second, 1);
}

TEST(hash_table, create_compare_hashtable_task_list) {
    std::vector<Filename> out_fn_list;
    CommonSubseqTaskList cstl;
    const std::size_t task_size = TASK_SIZE;
    const std::size_t hash_table_size = HASH_TABLE_SIZE;
    Filename out_fn_prefix = "sub_hash";

    create_compare_hashtable_task_list(out_fn_list, cstl, task_size,
                                       hash_table_size, out_fn_prefix);

    // assert output file name list
    EXPECT_EQ(out_fn_list.size(), task_size);

    for (std::size_t fn_index = 0; fn_index < out_fn_list.size(); fn_index++) {
        std::ostringstream oss;
        oss << out_fn_prefix << "_" << fn_index;
        EXPECT_EQ(out_fn_list[fn_index], oss.str());
    }

    // assert task information list
    EXPECT_EQ(cstl.size(), task_size);

    std::size_t task_index_size =
        static_cast<std::size_t>(static_cast<double>(hash_table_size) /
                                 static_cast<double>(cstl.size()));

    for (std::size_t cidx = 0; cidx < cstl.size(); ++cidx) {
        EXPECT_EQ(cstl[cidx].fn, out_fn_list[cidx]);
        EXPECT_EQ(cstl[cidx].begin, task_index_size * cidx);

        if (cidx < cstl.size() - 1) {
            EXPECT_EQ(cstl[cidx].end, task_index_size * (cidx + 1));
        } else {
            EXPECT_EQ(cstl[cidx].end, hash_table_size);
        }

        if (cidx > 0 && cidx < cstl.size() - 1) {
            EXPECT_EQ(cstl[cidx].begin, cstl[cidx - 1].end) << task_index_size;
        }
    }
}

void print_HashTable(const HashTable& ht) {
    for (std::size_t i = 0; i < ht.size(); ++i) {
        if (ht[i].size()) {
            std::cout << i << std::endl;
            for (auto& kv : ht[i]) {
                std::cout << "\t" << kv.first << " ";
                for (auto& loc : kv.second) {
                    std::cout << "(" << loc.first << ", " << loc.second << ") ";
                }
                std::cout << std::endl;
            }
        }
    }
}

TEST(hash_table, test_compare_hashtable_part) {
    Filename seq1_fn("./testdata/test_seq1.txt");
    Filename seq2_fn("./testdata/test_seq2.txt");

    std::shared_ptr<HashTable> phtx = create_hash_table(seq1_fn);
    std::shared_ptr<HashTable> phty = create_hash_table(seq2_fn);
    HashTable& htx = *phtx;
    HashTable& hty = *phty;

#if 0
    print_HashTable(htx);
    std::cout << std::endl << "-----" << std::endl;
    print_HashTable(hty);
#endif
    Filename out_fn("./testoutput/test_compare_hashtable_part.out");
    CommonSubseqTask cst(0, htx.size(), out_fn);

    compare_hashtable_part(cst, htx, hty);

    std::vector<ComSubseq> ans;
    ans.push_back(ComSubseq(1, 1, 2, 0));
    ans.push_back(ComSubseq(2, 1, 2, 0));
    ans.push_back(ComSubseq(0, 0, 1, 0));
    ans.push_back(ComSubseq(1, 0, 1, 0));
    ans.push_back(ComSubseq(2, 0, 1, 0));
    ans.push_back(ComSubseq(2, 1, 3, 1));

    std::vector<ComSubseq> csl;
    ComSubseqFileReader::readFile(out_fn, csl, 1);

    EXPECT_EQ(csl.size(), ans.size());
    for (std::size_t i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(csl[i], ans[i]) << i << " not the same";
    }
}

TEST(hash_table, test_compare_hashtable_part_2) {
    std::vector<Filename> out_fn_list;
    CommonSubseqTaskList cstl;
    Filename out_fn_prefix = "./testoutput/sub_hash";

    Filename seq1_fn("./testdata/test_seq1.txt");
    Filename seq2_fn("./testdata/test_seq2.txt");
    std::shared_ptr<HashTable> phtx = create_hash_table(seq1_fn);
    std::shared_ptr<HashTable> phty = create_hash_table(seq2_fn);
    HashTable& htx = *phtx;
    HashTable& hty = *phty;

#if 0
    print_HashTable(htx);
    std::cout << std::endl << "-----" << std::endl;
    print_HashTable(hty);
#endif

    create_compare_hashtable_task_list(out_fn_list, cstl, TASK_SIZE, htx.size(),
                                       out_fn_prefix);

    for (auto& cst : cstl) {
        compare_hashtable_part(cst, htx, hty);
    }

    std::vector<ComSubseq> csl;
    for (auto& out_fn : out_fn_list) {
        ComSubseqFileReader::readFile(out_fn, csl, 1);
    }

    std::vector<ComSubseq> ans;
    ans.push_back(ComSubseq(1, 1, 2, 0));
    ans.push_back(ComSubseq(2, 1, 2, 0));
    ans.push_back(ComSubseq(0, 0, 1, 0));
    ans.push_back(ComSubseq(1, 0, 1, 0));
    ans.push_back(ComSubseq(2, 0, 1, 0));
    ans.push_back(ComSubseq(2, 1, 3, 1));

    EXPECT_EQ(csl.size(), 6);
    for (std::size_t i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(csl[i], ans[i]) << i << " not the same";
    }
}

TEST(hash_table, test_common_subseq) {
    Filename seq1_fn("./testdata/test_seq1.txt");
    Filename seq2_fn("./testdata/test_seq2.txt");

    std::shared_ptr<std::vector<Filename>> pout_fn_list =
        common_subseq(seq1_fn, seq2_fn);

    std::vector<Filename>& out_fn_list = *pout_fn_list;

    std::vector<ComSubseq> csl;
    for (auto& out_fn : out_fn_list) {
        ComSubseqFileReader::readFile(out_fn, csl, 1);
    }

    std::vector<ComSubseq> ans;
    ans.push_back(ComSubseq(1, 1, 2, 0));
    ans.push_back(ComSubseq(2, 1, 2, 0));
    ans.push_back(ComSubseq(0, 0, 1, 0));
    ans.push_back(ComSubseq(1, 0, 1, 0));
    ans.push_back(ComSubseq(2, 0, 1, 0));
    ans.push_back(ComSubseq(2, 1, 3, 1));

    EXPECT_EQ(csl.size(), 6);
    for (std::size_t i = 0; i < ans.size(); ++i) {
        EXPECT_EQ(csl[i], ans[i]) << i << " not the same";
    }
}
}
#endif
