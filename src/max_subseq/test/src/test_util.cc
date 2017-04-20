#include <gtest/gtest.h>

#include "pcpe_util.h"

namespace pcpe {

TEST(pcpe_util, ChechFileExists) {
  ASSERT_TRUE(ChechFileExists("./testdata/test_seq1.txt"));
  ASSERT_TRUE(ChechFileExists("./testdata/test_seq2.txt"));

  ASSERT_FALSE(ChechFileExists("./does_not_exists"));
  ASSERT_FALSE(ChechFileExists("./testdata/does_not_exists"));
}

TEST(pcpe_util, GetFileSize) {
  ASSERT_EQ(GetFileSize("./testdata/test_seq1.txt"),      35);
  ASSERT_EQ(GetFileSize("./testdata/test_seq2.txt"),      21);
  ASSERT_EQ(GetFileSize("./testdata/test_esort_file.in"), 120);
  ASSERT_EQ(GetFileSize("./testdata/esort_input_1.in"),   40);
  ASSERT_EQ(GetFileSize("./testdata/esort_input_2.in"),   60);
  ASSERT_EQ(GetFileSize("./testdata/esort_input_3.in"),   20);

  ASSERT_EQ(GetFileSize("./does_not_exists"),             -1);
  ASSERT_EQ(GetFileSize("./testdata/does_not_exists"),    -1);
}

} // namespace pcpe
