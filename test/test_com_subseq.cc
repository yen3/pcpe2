#include <gtest/gtest.h>

#include <vector>

#include "com_subseq.h"
#include "env.h"

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
  ComSubseq x(1, 2, 3, 4, 6);
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

TEST(com_subseq, ReadComSubseqFile) {
  FilePath input_filename("./testdata/test_esort_file.in");

  std::vector<ComSubseq> com_subseqs;
  bool read_status = ReadComSubseqFile(input_filename, com_subseqs);
  ASSERT_TRUE(read_status);

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  CheckComSubseqsSame(com_subseqs, ans);
}

TEST(com_subseq, ReadComSubseqFile2) {
  FilePath input_filename("./testdata/not_existed");

  std::vector<ComSubseq> com_subseqs;
  bool read_status = ReadComSubseqFile(input_filename, com_subseqs);
  ASSERT_FALSE(read_status);
  ASSERT_TRUE(com_subseqs.empty());
}

TEST(com_subseq, ComSubseqFileReader) {
  FilePath ifn("./testdata/test_esort_file.in");  // input filename

  std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
  gEnv.setIOBufferSize(sizeof(ComSubseq) * 2);

  ComSubseqFileReader csfr(ifn);

  ASSERT_STREQ(csfr.getFilePath(), ifn.c_str());

  ASSERT_TRUE(csfr.is_open());
  ASSERT_FALSE(csfr.eof());

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  for (std::size_t i = 0; i < ans.size(); ++i) {
    ComSubseq read_seq;
    bool read_status = csfr.readSeq(read_seq);

    ASSERT_TRUE(read_status) << "read times: "<< i << std::endl;
    ASSERT_EQ(read_seq, ans[i]) << i << std::endl;

    // Check the open status
    if (i != ans.size() - 1) {
      ASSERT_TRUE(csfr.is_open()) << i << std::endl;
      ASSERT_FALSE(csfr.eof()) << i << std::endl;
    } else {
      ASSERT_FALSE(csfr.is_open());
      ASSERT_TRUE(csfr.eof());
    }
  }

  ASSERT_FALSE(csfr.is_open());
  ASSERT_TRUE(csfr.eof());

  gEnv.setIOBufferSize(savedIOBufferSize);
}

TEST(com_subseq, WriteComSubseqFile) {
  FilePath ifilepath("./testdata/test_esort_file.in");
  FilePath ofilepath(
      "./testoutput/test_ComSubseqFileWriter_writeFile.out");

  {
    std::vector<ComSubseq> com_subseqs;
    ASSERT_TRUE(ReadComSubseqFile(ifilepath, com_subseqs));
    ASSERT_TRUE(WriteComSubseqFile(com_subseqs, ofilepath));
  }

  std::vector<ComSubseq> com_subseqs;
  ReadComSubseqFile(ofilepath, com_subseqs);

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  CheckComSubseqsSame(com_subseqs, ans);
}

TEST(com_subseq, ComSubseqFileWriter) {
  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  FilePath ofilepath("./testoutput/test_ComSubseqFileWriter.out");

  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    gEnv.setIOBufferSize(sizeof(ComSubseq) * 2);

    ComSubseqFileWriter csfw(ofilepath);
    ASSERT_TRUE(csfw.is_open());

    for (const auto& cs: ans) {
      bool write_status = csfw.writeSeq(cs);

      ASSERT_TRUE(write_status);
      ASSERT_TRUE(csfw.is_open());
    }

    csfw.close();
    ASSERT_FALSE(csfw.is_open());

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  std::vector<ComSubseq> com_subseqs;
  ASSERT_TRUE(ReadComSubseqFile(ofilepath, com_subseqs));

  CheckComSubseqsSame(com_subseqs, ans);
}

TEST(com_subseq, ComSubseqFileWriter2) {
  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  FilePath ofilepath("./testoutput/test_ComSubseqFileWriter2.out");

  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    gEnv.setIOBufferSize(sizeof(ComSubseq) * 2);

    ComSubseqFileWriter csfw(ofilepath);
    ASSERT_TRUE(csfw.is_open());

    for (const auto& cs: ans) {
      bool write_status = csfw.writeSeq(cs);

      ASSERT_TRUE(write_status);
      ASSERT_TRUE(csfw.is_open());
    }

    csfw.close();
    ASSERT_FALSE(csfw.is_open());

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  std::vector<ComSubseq> com_subseqs;
  ComSubseqFileReader csfr(ofilepath);
  while (!csfr.eof()) {
    ComSubseq seq;

    bool read_status = csfr.readSeq(seq);
    ASSERT_TRUE(read_status);

    com_subseqs.push_back(seq);
  }

  CheckComSubseqsSame(com_subseqs, ans);
}

TEST(com_subseq, SplitComSubseqFile) {
  FilePath ifilepath("./testdata/test_esort_file.in");
  FilePath ofilepath("./testoutput/test_esort_file.in");

  std::vector<ComSubseq> com_subseqs;
  ASSERT_TRUE(ReadComSubseqFile(ifilepath, com_subseqs));
  ASSERT_TRUE(WriteComSubseqFile(com_subseqs, ofilepath));

  std::vector<FilePath> split_filepaths;
  SplitComSubseqFile(ofilepath, split_filepaths);

  ASSERT_EQ(split_filepaths.size(), 1);
  ASSERT_EQ(split_filepaths[0], ofilepath);
}

TEST(com_subseq, SplitComSubseqFile_small_buffer) {

  FilePath ifilepath("./testdata/test_esort_file.in");
  FilePath ofilepath("./testoutput/test_esort_file.in");

  {
    std::vector<ComSubseq> com_subseqs;
    ASSERT_TRUE(ReadComSubseqFile(ifilepath, com_subseqs));
    ASSERT_TRUE(WriteComSubseqFile(com_subseqs, ofilepath));
  }

  FileSize file_size;
  ASSERT_TRUE(GetFileSize(ofilepath.c_str(), file_size));
  ASSERT_EQ(file_size, sizeof(ComSubseq) * 6);

  std::vector<FilePath> split_filepaths;

  {
    std::size_t savedBufferSize = gEnv.getBufferSize();
    gEnv.setBufferSize(sizeof(ComSubseq) * 5);

    SplitComSubseqFile(ofilepath, split_filepaths);

    gEnv.setBufferSize(savedBufferSize);
  }

  ASSERT_EQ(split_filepaths.size(), 2);

  ASSERT_TRUE(GetFileSize(split_filepaths[0].c_str(), file_size));
  ASSERT_EQ(file_size, sizeof(ComSubseq) * 5);

  ASSERT_TRUE(GetFileSize(split_filepaths[1].c_str(), file_size));
  ASSERT_EQ(file_size, sizeof(ComSubseq) * 1);

  std::vector<ComSubseq> seqs;
  for (const auto& filepath : split_filepaths) {
    ComSubseqFileReader reader(filepath);

    while (!reader.eof()) {
      ComSubseq seq;
      ASSERT_TRUE(reader.readSeq(seq));
      seqs.push_back(seq);
    }

    reader.close();
  }

  std::vector<ComSubseq> ans;
  ReadComSubseqFile(ofilepath, ans);

  CheckComSubseqsSame(seqs, ans);
}

TEST(com_subseq, SplitComSubseqFile_small_buffer_2) {

  FilePath ifilepath("./testdata/test_esort_file.in");
  FilePath ofilepath("./testoutput/test_esort_file.in");

  {
    std::vector<ComSubseq> com_subseqs;
    ASSERT_TRUE(ReadComSubseqFile(ifilepath, com_subseqs));
    ASSERT_TRUE(WriteComSubseqFile(com_subseqs, ofilepath));
  }

  FileSize file_size;
  ASSERT_TRUE(GetFileSize(ofilepath.c_str(), file_size));
  ASSERT_EQ(file_size, sizeof(ComSubseq) * 6);

  std::vector<FilePath> split_filepaths;

  {
    std::size_t savedBufferSize = gEnv.getBufferSize();
    gEnv.setBufferSize(sizeof(ComSubseq) * 2);

    SplitComSubseqFile(ofilepath, split_filepaths);

    gEnv.setBufferSize(savedBufferSize);
  }

  ASSERT_EQ(split_filepaths.size(), 3);

  for (const auto& filepath : split_filepaths) {
    ASSERT_TRUE(GetFileSize(filepath.c_str(), file_size));
    ASSERT_EQ(file_size, sizeof(ComSubseq) * 2);
  }

  std::vector<ComSubseq> seqs;
  for (const auto& filepath : split_filepaths) {
    ComSubseqFileReader reader(filepath);

    while (!reader.eof()) {
      ComSubseq seq;
      ASSERT_TRUE(reader.readSeq(seq));
      seqs.push_back(seq);
    }

    reader.close();
  }

  std::vector<ComSubseq> ans;
  ReadComSubseqFile(ofilepath, ans);

  CheckComSubseqsSame(seqs, ans);
}

} // namespace pcpe
