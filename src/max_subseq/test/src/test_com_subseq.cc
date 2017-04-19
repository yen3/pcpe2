#include <gtest/gtest.h>

#include "com_subseq.h"

namespace pcpe {

TEST(test_com_subseq, length_fun) {
  ComSubseq x(1, 2, 3, 4);
  ASSERT_EQ(x.getLength(), 6);

  x.setLength(7);
  ASSERT_EQ(x.getLength(), 7);
}

TEST(test_com_subseq, compare_operator) {
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

TEST(test_com_subseq, continue_fun) {
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


} // namespace pcpe
