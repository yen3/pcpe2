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
  bool get_status;
  FileSize filesize;

  get_status = GetFileSize("./testdata/test_seq1.txt", filesize);
  ASSERT_TRUE(get_status);
  ASSERT_EQ(filesize, 35);

  get_status = GetFileSize("./testdata/test_seq2.txt", filesize);
  ASSERT_TRUE(get_status);
  ASSERT_EQ(filesize, 21);

  get_status = GetFileSize("./testdata/test_esort_file.in", filesize);
  ASSERT_TRUE(get_status);
  ASSERT_EQ(filesize, 120);

  get_status = GetFileSize("./testdata/esort_input_1.in", filesize);
  ASSERT_TRUE(get_status);
  ASSERT_EQ(filesize, 40);

  get_status = GetFileSize("./testdata/esort_input_2.in", filesize);
  ASSERT_TRUE(get_status);
  ASSERT_EQ(filesize, 60);

  get_status = GetFileSize("./testdata/esort_input_3.in", filesize);
  ASSERT_TRUE(get_status);
  ASSERT_EQ(filesize, 20);

  filesize = 0;

  get_status = GetFileSize("./does_not_exists", filesize);
  ASSERT_FALSE(get_status);
  ASSERT_EQ(filesize, 0);

  get_status = GetFileSize("./testdata/does_not_exists", filesize);
  ASSERT_FALSE(get_status);
  ASSERT_EQ(filesize, 0);
}

} // namespace pcpe
