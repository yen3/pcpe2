#include <gtest/gtest.h>

#include <cstring>

#include "seq.h"

namespace pcpe {

TEST(test_seq, basic_usage) {
  std::string base("ABCDEF");
  SmallSeq ss(base.c_str());

  ASSERT_EQ(std::strlen(ss.c_str()), ss.size());
  ASSERT_STREQ(ss.c_str(), base.c_str());
}

TEST(test_seq, basic_usage_long_string) {
  std::string base("ABCDEFGH");
  SmallSeq ss(base.c_str());

  std::string seq_base = base.substr(0, ss.size());

  ASSERT_EQ(std::strlen(ss.c_str()), ss.size());
  ASSERT_STREQ(ss.c_str(), seq_base.c_str());
}

TEST(test_seq, basic_usage_sub_string) {
  std::string base("ABCDEFGH");
  SmallSeq ss(base.c_str() + 1);

  std::string seq_base = base.substr(1, ss.size());

  ASSERT_EQ(std::strlen(ss.c_str()), ss.size());
  ASSERT_STREQ(ss.c_str(), seq_base.c_str());
}

TEST(test_seq, copy_seq) {
    std::string base("ABCDEF");
    SmallSeq ss1(base.c_str());
    SmallSeq ss2(ss1);

    ASSERT_EQ(ss1, ss2);
    ASSERT_STREQ(ss1.c_str(), ss2.c_str());
}

TEST(test_seq, assign_seq) {
    std::string base("ABCDEFG");
    std::string seq_base("ABCDEF");

    SmallSeq ss1(base.c_str());
    SmallSeq ss2(base.substr(1, ss1.size()).c_str());

    ASSERT_EQ(std::strlen(ss1.c_str()), ss1.size());
    ASSERT_EQ(std::strlen(ss2.c_str()), ss1.size());

    ASSERT_STREQ(ss1.c_str(), seq_base.c_str());

    ASSERT_NE(ss1, ss2);
    ASSERT_STRNE(ss1.c_str(), ss2.c_str());

    ss2 = ss1;
    ASSERT_STREQ(ss2.c_str(), seq_base.c_str());
    ASSERT_EQ(ss1, ss2);
    ASSERT_STREQ(ss1.c_str(), ss2.c_str());

    ASSERT_EQ(std::strlen(ss1.c_str()), ss1.size());
    ASSERT_EQ(std::strlen(ss2.c_str()), ss1.size());
}


} // namespace pcpe
