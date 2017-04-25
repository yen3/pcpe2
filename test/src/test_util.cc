#include <gtest/gtest.h>

#include <unistd.h>

#include "logging.h"
#include "pcpe_util.h"

namespace pcpe {

TEST(pcpe_util, CheckFileExists) {
  ASSERT_TRUE(CheckFileExists("./testdata/test_seq1.txt"));
  ASSERT_TRUE(CheckFileExists("./testdata/test_seq2.txt"));

  ASSERT_FALSE(CheckFileExists("./does_not_exist"));
  ASSERT_FALSE(CheckFileExists("./testdata/does_not_exist"));
}

TEST(pcpe_util, CheckFolderExists) {
  ASSERT_TRUE(CheckFolderExists("."));
  ASSERT_TRUE(CheckFolderExists("./"));
  ASSERT_TRUE(CheckFolderExists("../"));

  ASSERT_FALSE(CheckFolderExists("./does_not_exist"));
  ASSERT_FALSE(CheckFolderExists("./does_not_exist/does_not_exist"));
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

TEST(pcpe_util, CreateFolder_single_level) {
  InitLogging(LoggingLevel::kDebug);

	ASSERT_FALSE(CheckFolderExists("./testoutput/test_create_folder_single"));
  ASSERT_TRUE(CreateFolder("testoutput/test_create_folder_single"));
	ASSERT_TRUE(CheckFolderExists("./testoutput/test_create_folder_single"));

	rmdir("./testoutput/test_create_folder_single");

	ASSERT_FALSE(CheckFolderExists("temptemptemp"));
	ASSERT_TRUE(CreateFolder("temptemptemp"));
	EXPECT_TRUE(CheckFolderExists("temptemptemp"));

  rmdir("temptemptemp");
}

TEST(pcpe_util, CreateFolder_multi_level) {
  InitLogging(LoggingLevel::kDebug);

	ASSERT_FALSE(CheckFolderExists("./testoutput/test_create_folder"));
	ASSERT_FALSE(CheckFolderExists("./testoutput/test_create_folder/test1"));

  ASSERT_TRUE(CreateFolder("testoutput/test_create_folder/test1"));

	EXPECT_TRUE(CheckFolderExists("./testoutput/test_create_folder"));
	EXPECT_TRUE(CheckFolderExists("./testoutput/test_create_folder/test1"));

	rmdir("./testoutput/test_create_folder/test1");
	rmdir("./testoutput/test_create_folder");
}

} // namespace pcpe
