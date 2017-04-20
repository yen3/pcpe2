#include <gtest/gtest.h>

#include <vector>

#include "com_subseq.h"

namespace pcpe {

void CheckComSubseqsSame(const std::vector<ComSubseq>& cslx,
                         const std::vector<ComSubseq>& csly) {
    ASSERT_EQ(cslx.size(), csly.size());
    if (cslx.size() != csly.size())
      return;

    for (std::size_t i = 0; i < cslx.size(); ++i) {
        ASSERT_EQ(cslx[i], csly[i]);
    }
}

TEST(com_subseq, length_fun) {
  ComSubseq x(1, 2, 3, 4);
  ASSERT_EQ(x.getLength(), 6);

  x.setLength(7);
  ASSERT_EQ(x.getLength(), 7);
}

TEST(com_subseq, compare_operator) {
  ComSubseq x(1, 2, 3, 4, 7);
  ComSubseq y(2, 3, 3, 4, 7);
  ComSubseq z(1, 2, 3, 4, 7);

  ASSERT_TRUE(x == z);
  ASSERT_FALSE(x == y);

  ASSERT_TRUE(x != y);
  ASSERT_FALSE(x != z);

  ASSERT_TRUE(x < y);
  ASSERT_FALSE(x < z);

  ASSERT_TRUE(x <= z);
  ASSERT_TRUE(z <= x);

  ASSERT_TRUE(y > x);
  ASSERT_TRUE(y >= x);
}

TEST(com_subseq, continue_fun) {
  ComSubseq x(1, 2, 3, 4, 6);
  ComSubseq y(1, 2, 3, 4, 7);
  ComSubseq z(1, 2, 4, 5, 6);
  ComSubseq u(2, 2, 5, 6, 7);

  ASSERT_TRUE(x.isSameSeqeunce(y));
  ASSERT_TRUE(y.isSameSeqeunce(x));

  ASSERT_TRUE(x.isSameSeqeunce(z));
  ASSERT_TRUE(z.isSameSeqeunce(x));

  ASSERT_FALSE(x.isSameSeqeunce(u));
  ASSERT_FALSE(u.isSameSeqeunce(x));

  ASSERT_TRUE(x.isContinued(z));
  ASSERT_TRUE(z.isContinued(x));

  ASSERT_FALSE(x.isContinued(y));
  ASSERT_FALSE(y.isContinued(x));
}

TEST(com_subseq, ReadComSubSeqFile) {
  FilePath input_filename("./testdata/test_esort_file.in");

  std::vector<ComSubseq> com_subseqs;
  bool read_status = ReadComSubSeqFile(input_filename, com_subseqs);
  ASSERT_TRUE(read_status);


  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0));
  ans.push_back(ComSubseq(2, 1, 2, 0));
  ans.push_back(ComSubseq(0, 0, 1, 0));
  ans.push_back(ComSubseq(1, 0, 1, 0));
  ans.push_back(ComSubseq(2, 0, 1, 0));
  ans.push_back(ComSubseq(2, 1, 3, 1));

  CheckComSubseqsSame(com_subseqs, ans);
}

TEST(com_subseq, ReadComSubSeqFile2) {
  FilePath input_filename("./testdata/not_existed");

  std::vector<ComSubseq> com_subseqs;
  bool read_status = ReadComSubSeqFile(input_filename, com_subseqs);
  ASSERT_FALSE(read_status);
  ASSERT_TRUE(com_subseqs.empty());
}

TEST(com_subseq, ComSubseqFileReader) {
  FilePath ifn("./testdata/test_esort_file.in");  // input filename

  ComSubseqFileReader csfr(ifn, sizeof(ComSubseq) * 2);

  ASSERT_STREQ(csfr.filepath(), ifn.c_str());

  ASSERT_TRUE(csfr.is_open());
  ASSERT_FALSE(csfr.fail());
  ASSERT_FALSE(csfr.eof());

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0));
  ans.push_back(ComSubseq(2, 1, 2, 0));
  ans.push_back(ComSubseq(0, 0, 1, 0));
  ans.push_back(ComSubseq(1, 0, 1, 0));
  ans.push_back(ComSubseq(2, 0, 1, 0));
  ans.push_back(ComSubseq(2, 1, 3, 1));

  for (std::size_t i = 0; i < ans.size(); ++i) {
    ComSubseq read_seq;
    bool read_status = csfr.readSeq(read_seq);

    ASSERT_TRUE(read_status);
    ASSERT_EQ(read_seq, ans[i]) << i << std::endl;

    // Check the open status
    if (i != ans.size() - 1) {
      ASSERT_TRUE(csfr.is_open()) << i << std::endl;
      ASSERT_FALSE(csfr.fail()) << i << std::endl;
      ASSERT_FALSE(csfr.eof()) << i << std::endl;
    } else {
      ASSERT_FALSE(csfr.is_open());
      ASSERT_FALSE(csfr.fail());
      ASSERT_TRUE(csfr.eof());
    }
  }

  ASSERT_FALSE(csfr.is_open());
  ASSERT_FALSE(csfr.fail());
  ASSERT_TRUE(csfr.eof());
}

TEST(com_subseq, ComSubseqFileReader_fail) {
}


} // namespace pcpe
