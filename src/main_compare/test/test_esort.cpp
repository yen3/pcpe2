#if defined(__GTEST_PCPE__)

#include "hash_table.h"
#include "pcpe_util.h"
#include "esort.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include <gtest/gtest.h>

namespace pcpe{

extern void esort_sort_file(const Filename& fn);
extern void esort_sort_file_task(const FilenameList& esfl);
extern void esort_sort_files(const FilenameList& fn_list);
extern void esort_merge_sort_files(const FilenameList& fn_list,
                                   const Filename& esort_fn,
                                   const std::size_t kBufferSize);
extern void esort(std::shared_ptr<FilenameList> fn_list,
                  const Filename esort_fn);

void copy_seq_file(const Filename& ifn, const Filename& ofn)
{
    std::vector<ComSubseq> icsl;
    std::vector<ComSubseq> ocsl;

    // copy file
    ComSubseqFileReader::readFile(ifn, icsl);
    ComSubseqFileWriter::writeFile(ofn, icsl);

    ComSubseqFileReader::readFile(ofn, ocsl);

    EXPECT_EQ(icsl.size(), ocsl.size());
    for(std::size_t i=0; i<icsl.size(); ++i){
        EXPECT_EQ(icsl[i], ocsl[i]);
    }
}

void assert_comsubseq_list(const ComSubseqList& cslx, const ComSubseqList& csly)
{
    EXPECT_EQ(cslx.size(), csly.size());

    for(std::size_t i=0; i<cslx.size(); ++i){
        EXPECT_EQ(cslx[i], csly[i]);   
    }
}

TEST(esort, test_esort_sort_file){
    Filename oifn("./testdata/test_esort_file.in");
    Filename ifn("./testoutput/test_esort_file.out");

    // copy oifn file to ifn since the esort_sort_file would sort the file
    // and write back
    copy_seq_file(oifn, ifn);

    esort_sort_file(ifn);

    std::vector<ComSubseq> csl_sorted;
    ComSubseqFileReader::readFile(ifn, csl_sorted);

    for(std::size_t i=0; i<csl_sorted.size()-1; ++i){
        EXPECT_TRUE(csl_sorted[i] < csl_sorted[i+1]);
    }

    std::vector<ComSubseq> ans;
    ans.push_back(ComSubseq(0, 0, 1, 0));
    ans.push_back(ComSubseq(1, 0, 1, 0));
    ans.push_back(ComSubseq(1, 1, 2, 0));
    ans.push_back(ComSubseq(2, 0, 1, 0));
    ans.push_back(ComSubseq(2, 1, 2, 0));
    ans.push_back(ComSubseq(2, 1, 3, 1));

    assert_comsubseq_list(csl_sorted, ans);
#if 0
    for(auto& cs: csl_sorted){
        cs.print();
    }
#endif
}

TEST(esort, test_esort_sort_file_2){
    copy_seq_file("./testdata/esort_input_1.in", "./testoutput/esort_input_1.out");
    copy_seq_file("./testdata/esort_input_2.in", "./testoutput/esort_input_2.out");
    copy_seq_file("./testdata/esort_input_3.in", "./testoutput/esort_input_3.out");

    FilenameList ifn_list;
    ifn_list.push_back("./testoutput/esort_input_1.out");
    ifn_list.push_back("./testoutput/esort_input_2.out");
    ifn_list.push_back("./testoutput/esort_input_3.out");

    std::vector<ComSubseqList> ans_list(ifn_list.size());
    ans_list[0].push_back(ComSubseq(1, 1, 2, 0));
    ans_list[0].push_back(ComSubseq(2, 1, 2, 0));

    ans_list[1].push_back(ComSubseq(0, 0, 1, 0));
    ans_list[1].push_back(ComSubseq(1, 0, 1, 0));
    ans_list[1].push_back(ComSubseq(2, 0, 1, 0));

    ans_list[2].push_back(ComSubseq(2, 1, 3, 1));

    for(std::size_t i=0; i<ifn_list.size(); ++i){
        esort_sort_file(ifn_list[i]);

        ComSubseqList csl;
        ComSubseqFileReader::readFile(ifn_list[i], csl);
        assert_comsubseq_list(csl, ans_list[i]);

    }
}


TEST(esort, test_esort_sort_files){
    copy_seq_file("./testdata/esort_input_1.in", "./testoutput/esort_output_1.out");
    copy_seq_file("./testdata/esort_input_2.in", "./testoutput/esort_output_2.out");
    copy_seq_file("./testdata/esort_input_3.in", "./testoutput/esort_output_3.out");

    FilenameList ifn_list;
    ifn_list.push_back("./testoutput/esort_output_1.out");
    ifn_list.push_back("./testoutput/esort_output_2.out");
    ifn_list.push_back("./testoutput/esort_output_3.out");

    esort_sort_files(ifn_list);
    
    std::vector<ComSubseqList> ans_list(ifn_list.size());
    ans_list[0].push_back(ComSubseq(1, 1, 2, 0));
    ans_list[0].push_back(ComSubseq(2, 1, 2, 0));

    ans_list[1].push_back(ComSubseq(0, 0, 1, 0));
    ans_list[1].push_back(ComSubseq(1, 0, 1, 0));
    ans_list[1].push_back(ComSubseq(2, 0, 1, 0));

    ans_list[2].push_back(ComSubseq(2, 1, 3, 1));

    for(std::size_t i=0; i<ifn_list.size(); ++i){
        ComSubseqList csl;
        ComSubseqFileReader::readFile(ifn_list[i], csl);
        assert_comsubseq_list(csl, ans_list[i]);
    }
}

TEST(esort, test_esort_sort_files_2){
    Filename seq1_fn("./testdata/test_seq1.txt");
    Filename seq2_fn("./testdata/test_seq2.txt");

    std::shared_ptr<std::vector<Filename> > pout_fn_list = 
        common_subseq(seq1_fn, seq2_fn);

    std::vector<Filename>& out_fn_list = *pout_fn_list;

    esort_sort_files(out_fn_list);

    std::vector<ComSubseqList> ans_list(out_fn_list.size());
    ans_list[6].push_back(ComSubseq(1, 1, 2, 0));
    ans_list[6].push_back(ComSubseq(2, 1, 2, 0));
    ans_list[50].push_back(ComSubseq(0, 0, 1, 0));
    ans_list[50].push_back(ComSubseq(1, 0, 1, 0));
    ans_list[50].push_back(ComSubseq(2, 0, 1, 0));
    ans_list[63].push_back(ComSubseq(2, 1, 3, 1));

    for(std::size_t i=0; i< ans_list.size(); ++i){
        if(ans_list[i].size()){
            ComSubseqList csl;
            ComSubseqFileReader::readFile(out_fn_list[i], csl);
            assert_comsubseq_list(csl, ans_list[i]);
        }
    }
}

TEST(esort, test_esort_merge_sort_files){
    copy_seq_file("./testdata/esort_input_1.in", "./testoutput/esort_output_1.out");
    copy_seq_file("./testdata/esort_input_2.in", "./testoutput/esort_output_2.out");
    copy_seq_file("./testdata/esort_input_3.in", "./testoutput/esort_output_3.out");

    FilenameList ifn_list;
    ifn_list.push_back("./testoutput/esort_output_1.out");
    ifn_list.push_back("./testoutput/esort_output_2.out");
    ifn_list.push_back("./testoutput/esort_output_3.out");

    esort_sort_files(ifn_list);

    esort_merge_sort_files(ifn_list, "./testoutput/esort_merged_output.out", 2);

    ComSubseqList csl;
    ComSubseqFileReader::readFile("./testoutput/esort_merged_output.out", csl);

#if 0
    std::cout << std::endl << std::endl;
    for(auto& cs: csl){
        cs.print();
    }
    std::cout << std::endl << std::endl;
#endif

    std::vector<ComSubseq> ans;
    ans.push_back(ComSubseq(0, 0, 1, 0));
    ans.push_back(ComSubseq(1, 0, 1, 0));
    ans.push_back(ComSubseq(1, 1, 2, 0));
    ans.push_back(ComSubseq(2, 0, 1, 0));
    ans.push_back(ComSubseq(2, 1, 2, 0));
    ans.push_back(ComSubseq(2, 1, 3, 1));

    assert_comsubseq_list(csl, ans);
}

}

#endif /* __GTEST_PCPE__ */
