#if defined(__GTEST_PCPE__)

#include <gtest/gtest.h>

#include "pcpe_util.h"

namespace pcpe {

extern void assert_comsubseq_list(const ComSubseqList& cslx,
                                  const ComSubseqList& csly);

TEST(pcpe_util, test_get_filesize) {
    EXPECT_EQ(get_filesize("./testdata/test_seq1.txt"), 35);
    EXPECT_EQ(get_filesize("./testdata/test_seq2.txt"), 21);
    EXPECT_EQ(get_filesize("./testdata/test_esort_file.in"), 120);
    EXPECT_EQ(get_filesize("./testdata/esort_input_1.in"), 40);
    EXPECT_EQ(get_filesize("./testdata/esort_input_2.in"), 60);
    EXPECT_EQ(get_filesize("./testdata/esort_input_3.in"), 20);
}

TEST(pcpe_util, test_ComSubseq_operator) {
    ComSubseq x(1, 2, 3, 4, 7);
    ComSubseq y(2, 3, 3, 4, 7);
    ComSubseq z(1, 2, 3, 4, 7);

    EXPECT_TRUE(x == z);
    EXPECT_FALSE(x == y);

    EXPECT_TRUE(x != y);
    EXPECT_FALSE(x != z);

    EXPECT_TRUE(x < y);
    EXPECT_FALSE(x < z);

    EXPECT_TRUE(x <= z);
    EXPECT_TRUE(z <= x);

    EXPECT_TRUE(y > x);
    EXPECT_TRUE(y >= x);
}

TEST(pcpe_util, test_ComSubseq_length) {
    ComSubseq x(1, 2, 3, 4);
    EXPECT_EQ(x.getLength(), 6);

    x.setLength(7);
    EXPECT_EQ(x.getLength(), 7);
}

TEST(pcpe_util, test_ComSubseq_SequenceFunction) {
    ComSubseq x(1, 2, 3, 4, 6);
    ComSubseq y(1, 2, 3, 4, 7);
    ComSubseq z(1, 2, 4, 5, 6);
    ComSubseq u(2, 2, 5, 6, 7);

    EXPECT_TRUE(x.isSameSeqeunce(y));
    EXPECT_TRUE(y.isSameSeqeunce(x));

    EXPECT_TRUE(x.isSameSeqeunce(z));
    EXPECT_TRUE(z.isSameSeqeunce(x));

    EXPECT_FALSE(x.isSameSeqeunce(u));
    EXPECT_FALSE(u.isSameSeqeunce(x));

    EXPECT_TRUE(x.isContinued(z));
    EXPECT_TRUE(z.isContinued(x));

    EXPECT_FALSE(x.isContinued(y));
    EXPECT_FALSE(y.isContinued(x));
}

TEST(pcpe_util, test_ComSubseqFileReader_readFile) {
    Filename input_filename("./testdata/test_esort_file.in");

    std::vector<ComSubseq> com_seq_list;
    ComSubseqFileReader::readFile(input_filename, com_seq_list);

    std::vector<ComSubseq> ans_list;
    ans_list.push_back(ComSubseq(1, 1, 2, 0));
    ans_list.push_back(ComSubseq(2, 1, 2, 0));
    ans_list.push_back(ComSubseq(0, 0, 1, 0));
    ans_list.push_back(ComSubseq(1, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 1, 3, 1));

    assert_comsubseq_list(com_seq_list, ans_list);
}


TEST(pcpe_util, test_ComSubseqFileReader) {
    Filename ifn("./testdata/test_esort_file.in");  // input filename

    ComSubseqFileReader csfr(ifn);

#if defined(__DEBUG__)
    EXPECT_EQ(csfr.getFn(), ifn);
#endif

    EXPECT_TRUE(csfr.is_open());
    EXPECT_FALSE(csfr.eof());

    std::vector<ComSubseq> ans_list;
    ans_list.push_back(ComSubseq(1, 1, 2, 0));
    ans_list.push_back(ComSubseq(2, 1, 2, 0));
    ans_list.push_back(ComSubseq(0, 0, 1, 0));
    ans_list.push_back(ComSubseq(1, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 1, 3, 1));

    for (auto i = 0; i < ans_list.size(); ++i) {
        ComSubseq read_seq;
        csfr.readSeq(read_seq);

        EXPECT_EQ(read_seq, ans_list[i]) << i << std::endl;

        if (i != ans_list.size() - 1) {
            EXPECT_TRUE(csfr.is_open()) << i << std::endl;
            EXPECT_FALSE(csfr.eof()) << i << std::endl;
        } else{
            EXPECT_FALSE(csfr.is_open());
            EXPECT_TRUE(csfr.eof());
        }
    }

    EXPECT_FALSE(csfr.is_open());
    EXPECT_TRUE(csfr.eof());
}

TEST(pcpe_util, test_ComSubseqFileWriter_writeFile) {
    Filename input_filename("./testdata/test_esort_file.in");
    Filename output_filename(
        "./testoutput/test_ComSubseqFileWriter_writeFile.out");

    {
        std::vector<ComSubseq> com_seq_list;
        ComSubseqFileReader::readFile(input_filename, com_seq_list);
        ComSubseqFileWriter::writeFile(output_filename, com_seq_list);
    }


    std::vector<ComSubseq> com_seq_list;
    ComSubseqFileReader::readFile(output_filename, com_seq_list);

    std::vector<ComSubseq> ans_list;
    ans_list.push_back(ComSubseq(1, 1, 2, 0));
    ans_list.push_back(ComSubseq(2, 1, 2, 0));
    ans_list.push_back(ComSubseq(0, 0, 1, 0));
    ans_list.push_back(ComSubseq(1, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 1, 3, 1));

    assert_comsubseq_list(com_seq_list, ans_list);
}

TEST(pcpe_util, test_ComSubseqFileWriter) {
    std::vector<ComSubseq> ans_list;
    ans_list.push_back(ComSubseq(1, 1, 2, 0));
    ans_list.push_back(ComSubseq(2, 1, 2, 0));
    ans_list.push_back(ComSubseq(0, 0, 1, 0));
    ans_list.push_back(ComSubseq(1, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 1, 3, 1));

    Filename output_filename("./testoutput/test_ComSubseqFileWriter.out");

    {
        ComSubseqFileWriter csfw(output_filename);
        EXPECT_TRUE(csfw.is_open());

        for(const auto& cs: ans_list){
            csfw.writeSeq(cs);

            EXPECT_TRUE(csfw.is_open());
        }

        csfw.close();
        EXPECT_FALSE(csfw.is_open());
    }


    std::vector<ComSubseq> com_seq_list;
    ComSubseqFileReader::readFile(output_filename, com_seq_list);

    assert_comsubseq_list(com_seq_list, ans_list);
}

TEST(pcpe_util, test_ComSubseqFileWriter2) {
    std::vector<ComSubseq> ans_list;
    ans_list.push_back(ComSubseq(1, 1, 2, 0));
    ans_list.push_back(ComSubseq(2, 1, 2, 0));
    ans_list.push_back(ComSubseq(0, 0, 1, 0));
    ans_list.push_back(ComSubseq(1, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 0, 1, 0));
    ans_list.push_back(ComSubseq(2, 1, 3, 1));

    Filename output_filename("./testoutput/test_ComSubseqFileWriter.out");

    {
        ComSubseqFileWriter csfw(output_filename);
        EXPECT_TRUE(csfw.is_open());

        for(const auto& cs: ans_list){
            csfw.writeSeq(cs);

            EXPECT_TRUE(csfw.is_open());
        }

        csfw.close();
        EXPECT_FALSE(csfw.is_open());
    }

    std::vector<ComSubseq> com_seq_list;
    ComSubseqFileReader csfr(output_filename);
    while(!csfr.eof()){
        ComSubseq seq;
        csfr.readSeq(seq);
        com_seq_list.push_back(seq);
    }

    assert_comsubseq_list(com_seq_list, ans_list);

}


}
#endif
