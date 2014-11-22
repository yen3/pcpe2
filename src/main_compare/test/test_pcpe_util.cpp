#if defined(__GTEST_PCPE__)

#include <gtest/gtest.h>

#include "pcpe_util.h"

namespace pcpe {

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
}
#endif
