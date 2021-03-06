#include <gtest/gtest.h>

#include <algorithm>
#include <iostream>

#include "com_subseq.h"
#include "env.h"
#include "logging.h"
#include "pcpe_util.h"
#include "small_seq_hash.h"

namespace pcpe {

extern void ReadSequences(const FilePath& filepath, SeqList& seqs);
extern void ConstructSmallSeqHash(const FilePath& filepath,
                                  std::vector<FilePath>& hash_filepaths);
extern void ConstructSmallSeqs(const SeqList& seqs, std::size_t seqs_begin,
                               std::size_t seqs_end, SmallSeqList& smallseqs);
extern void CompareSmallSeqHash(const std::vector<FilePath>& x_filepaths,
                                const std::vector<FilePath>& y_filepaths,
                                std::vector<FilePath>& result_filepaths);

bool IsSameSmallSeqs(const SmallSeqList& xs, const SmallSeqList& ys) {
  if (xs.size() != ys.size()) return false;

  auto xs_iter_end = xs.end();
  auto ys_iter_end = ys.end();

  for (auto xs_iter = xs.begin(), ys_iter = ys.begin();
       xs_iter != xs_iter_end && ys_iter != ys_iter_end; ++xs_iter, ++ys_iter) {
    if (xs_iter->first != ys_iter->first) return false;

    if (xs_iter->second.size() != ys_iter->second.size()) return false;

    for (std::size_t i = 0; i < xs_iter->second.size(); ++i) {
      if (xs_iter->second[i].idx != ys_iter->second[i].idx ||
          xs_iter->second[i].loc != ys_iter->second[i].loc)
        return false;
    }
  }

  return true;
}

void ReadSmallSeqs(const FilePath& ifilepath, SmallSeqList& ss) {
  SmallSeqHashFileReader reader(ifilepath);
  while (!reader.eof()) {
    SmallSeqHashIndex index;
    SeqLocList value;
    reader.readEntry(index, value);

    for (const auto& v : value) {
      ss[index].emplace_back(v);
    }
  }
  reader.close();
}

void WriteSmallSeqs(const SmallSeqList& ss, const FilePath& ofilepath) {
  SmallSeqHashFileWriter writer(ofilepath);
  for (const auto& kv : ss) {
    writer.writeEntry(kv);
  }
  writer.close();
}

TEST(compare_subseq, test_hash_value) {
  EXPECT_EQ(0UL, HashSmallSeq("AAAAAA"));
  EXPECT_EQ(1UL, HashSmallSeq("BAAAAA"));
  EXPECT_EQ(27UL, HashSmallSeq("BBAAAA"));
  EXPECT_EQ(703UL, HashSmallSeq("BBBAAA"));

  EXPECT_EQ(61288890UL, HashSmallSeq("ABCDEF"));
  EXPECT_EQ(73645521UL, HashSmallSeq("BCDEFG"));
  EXPECT_EQ(86002152UL, HashSmallSeq("CDEFGH"));
  EXPECT_EQ(98358783UL, HashSmallSeq("DEFGHI"));
}

TEST(compare_subseq, test_small_hash_table_reader) {
  SmallSeqList seqs;
  seqs[0].emplace_back(SeqLoc(1, 0));
  seqs[2].emplace_back(SeqLoc(2, 0));
  seqs[2].emplace_back(SeqLoc(5, 0));
  seqs[3].emplace_back(SeqLoc(3, 0));
  seqs[4].emplace_back(SeqLoc(0, 0));

  const FilePath input_path = "testdata/test_small_hash_table_reader_1.in";

  SmallSeqList read_seqs;
  ReadSmallSeqs(input_path, read_seqs);

  ASSERT_TRUE(IsSameSmallSeqs(seqs, read_seqs));
}

TEST(compare_subseq, test_small_hash_table_reader_small_buffer) {
  SmallSeqList seqs;
  seqs[0].emplace_back(SeqLoc(1, 0));
  seqs[2].emplace_back(SeqLoc(2, 0));
  seqs[2].emplace_back(SeqLoc(5, 0));
  seqs[3].emplace_back(SeqLoc(3, 0));
  seqs[4].emplace_back(SeqLoc(0, 0));

  const FilePath input_path = "testdata/test_small_hash_table_reader_1.in";

  SmallSeqList read_seqs;
  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    gEnv.setIOBufferSize(48);

    ReadSmallSeqs(input_path, read_seqs);

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  ASSERT_TRUE(IsSameSmallSeqs(seqs, read_seqs));
}

TEST(compare_subseq, test_small_hash_table_reader_minimal_buffer) {
  SmallSeqList seqs;
  seqs[0].emplace_back(SeqLoc(1, 0));
  seqs[2].emplace_back(SeqLoc(2, 0));
  seqs[2].emplace_back(SeqLoc(5, 0));
  seqs[3].emplace_back(SeqLoc(3, 0));
  seqs[4].emplace_back(SeqLoc(0, 0));

  const FilePath input_path = "testdata/test_small_hash_table_reader_1.in";

  SmallSeqList read_seqs;
  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    gEnv.setIOBufferSize(sizeof(SmallSeqHashIndex) + sizeof(uint32_t) +
                         sizeof(SeqLoc));

    ReadSmallSeqs(input_path, read_seqs);

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  ASSERT_TRUE(IsSameSmallSeqs(seqs, read_seqs));
}

TEST(compare_subseq, test_small_hash_table_writer) {
  // Read the seqs
  SmallSeqList seqs;
  ReadSmallSeqs("testdata/test_small_hash_table_reader_1.in", seqs);

  // Write the sequences
  const FilePath output_path = "testoutput/test_small_hash_table_writer";
  WriteSmallSeqs(seqs, output_path);

  // Check file size
  FileSize output_file_size;
  bool check_state = GetFileSize(output_path.c_str(), output_file_size);
  ASSERT_EQ(check_state, true);
  ASSERT_EQ(output_file_size, 72);

  // Read it back again and comapre
  SmallSeqList read_seqs;
  ReadSmallSeqs(output_path, read_seqs);
  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_small_hash_table_writer_small_buffer) {
  // Read the seqs
  SmallSeqList seqs;
  ReadSmallSeqs("testdata/test_small_hash_table_reader_1.in", seqs);

  // Write the sequences
  const FilePath output_path = "testoutput/test_small_hash_table_writer";
  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    gEnv.setIOBufferSize(sizeof(SmallSeqHashIndex) + sizeof(uint32_t) +
                         sizeof(SeqLoc));

    WriteSmallSeqs(seqs, output_path);

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  // Check file size
  FileSize output_file_size;
  bool check_state = GetFileSize(output_path.c_str(), output_file_size);
  ASSERT_EQ(check_state, true);
  ASSERT_EQ(output_file_size, 72);

  // Read it back again and comapre
  SmallSeqList read_seqs;
  ReadSmallSeqs(output_path, read_seqs);
  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_small_hash_table_writer_minimal_buffer) {
  // Read the seqs
  SmallSeqList seqs;
  ReadSmallSeqs("testdata/test_small_hash_table_reader_1.in", seqs);

  // Write the sequences
  const FilePath output_path = "testoutput/test_small_hash_table_writer";
  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    // gEnv.setIOBufferSize(sizeof(SmallSeqHashIndex) + sizeof(uint32_t) +
    // sizeof(SeqLoc));

    gEnv.setIOBufferSize(sizeof(uint32_t));

    WriteSmallSeqs(seqs, output_path);

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  // Check file size
  FileSize output_file_size;
  bool check_state = GetFileSize(output_path.c_str(), output_file_size);
  ASSERT_EQ(check_state, true);
  ASSERT_EQ(output_file_size, 72);

  // Read it back again and comapre
  SmallSeqList read_seqs;
  ReadSmallSeqs(output_path, read_seqs);
  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_read_smallseqs_2_1) {
  FilePath filepath = "testdata/test_seq1.txt";
  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  // SmallSeqLocList seqs;
  SmallSeqList seqs;
  ConstructSmallSeqs(seq_list, 0, seq_list.size(), seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 4UL);

  // Check keys exist
  ASSERT_NE(seqs.find(HashSmallSeq("ABCDEF")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("BCDEFG")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("CDEFGH")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("DEFGHI")), seqs.end());

  // Check all idx& locations are saved
  ASSERT_EQ(seqs[HashSmallSeq("ABCDEF")].size(), 3UL);
  ASSERT_EQ(seqs[HashSmallSeq("BCDEFG")].size(), 3UL);
  ASSERT_EQ(seqs[HashSmallSeq("CDEFGH")].size(), 2UL);
  ASSERT_EQ(seqs[HashSmallSeq("DEFGHI")].size(), 1UL);

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("ABCDEF")];

    ASSERT_EQ(idxlocs.size(), 3UL);

    ASSERT_EQ(idxlocs[0].idx, 0U);
    ASSERT_EQ(idxlocs[0].loc, 0U);
    ASSERT_EQ(idxlocs[1].idx, 1U);
    ASSERT_EQ(idxlocs[1].loc, 0U);
    ASSERT_EQ(idxlocs[2].idx, 2U);
    ASSERT_EQ(idxlocs[2].loc, 0U);
  }

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 3UL);

    ASSERT_EQ(idxlocs[0].idx, 0U);
    ASSERT_EQ(idxlocs[0].loc, 1U);
    ASSERT_EQ(idxlocs[1].idx, 1U);
    ASSERT_EQ(idxlocs[1].loc, 1U);
    ASSERT_EQ(idxlocs[2].idx, 2U);
    ASSERT_EQ(idxlocs[2].loc, 1U);
  }

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 2UL);

    ASSERT_EQ(idxlocs[0].idx, 1U);
    ASSERT_EQ(idxlocs[0].loc, 2U);
    ASSERT_EQ(idxlocs[1].idx, 2U);
    ASSERT_EQ(idxlocs[1].loc, 2U);
  }

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 2U);
    ASSERT_EQ(idxlocs[0].loc, 3U);
  }
}

TEST(compare_subseq, test_read_smallseqs_2_2) {
  FilePath filepath = "testdata/test_seq2.txt";

  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  SmallSeqList seqs;
  ConstructSmallSeqs(seq_list, 0, seq_list.size(), seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 3UL);

  // Check keys exist
  ASSERT_NE(seqs.find(HashSmallSeq("BCDEFG")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("CDEFGH")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("DEFGHI")), seqs.end());

  // Check all idx& locations are saved
  ASSERT_EQ(seqs[HashSmallSeq("BCDEFG")].size(), 1UL);
  ASSERT_EQ(seqs[HashSmallSeq("CDEFGH")].size(), 1UL);
  ASSERT_EQ(seqs[HashSmallSeq("DEFGHI")].size(), 1UL);

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 0U);
    ASSERT_EQ(idxlocs[0].loc, 0U);
  }

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 1U);
    ASSERT_EQ(idxlocs[0].loc, 0U);
  }

  {
    SeqLocList& idxlocs = seqs[HashSmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 1U);
    ASSERT_EQ(idxlocs[0].loc, 1U);
  }
}

TEST(compare_subseq, test_construct_small_seq_hash_files_1) {
  FilePath filepath = "testdata/test_seq1.txt";

  std::vector<FilePath> ht_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput/");

    ConstructSmallSeqHash(filepath, ht_paths);

    gEnv.setTempFolderPath(saved_temp);
  }

  ASSERT_EQ(1UL, ht_paths.size());

  const FilePath& ofilepath = ht_paths[0];
  ASSERT_TRUE(CheckFileExists(ofilepath.c_str()));

  SmallSeqList seqs;
  seqs[61288890].emplace_back(SeqLoc(0, 0));
  seqs[61288890].emplace_back(SeqLoc(1, 0));
  seqs[61288890].emplace_back(SeqLoc(2, 0));

  seqs[73645521].emplace_back(SeqLoc(0, 1));
  seqs[73645521].emplace_back(SeqLoc(1, 1));
  seqs[73645521].emplace_back(SeqLoc(2, 1));

  seqs[86002152].emplace_back(SeqLoc(1, 2));
  seqs[86002152].emplace_back(SeqLoc(2, 2));

  seqs[98358783].emplace_back(SeqLoc(2, 3));

  SmallSeqList read_seqs;
  ReadSmallSeqs(ofilepath, read_seqs);

  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_construct_small_seq_hash_files_2) {
  FilePath filepath = "testdata/test_seq1.txt";

  std::vector<FilePath> ht_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    uint32_t saved_compare_seq_size = gEnv.getCompareSeqenceSize();

    gEnv.setTempFolderPath("testoutput/");
    gEnv.setCompareSeqenceSize(2);

    ConstructSmallSeqHash(filepath, ht_paths);

    gEnv.setCompareSeqenceSize(saved_compare_seq_size);
    gEnv.setTempFolderPath(saved_temp);
  }

  ASSERT_EQ(2UL, ht_paths.size());
  {
    const FilePath& ofilepath = ht_paths[0];

    SmallSeqList seqs;
    seqs[61288890].emplace_back(SeqLoc(0, 0));
    seqs[61288890].emplace_back(SeqLoc(1, 0));
    seqs[73645521].emplace_back(SeqLoc(0, 1));
    seqs[73645521].emplace_back(SeqLoc(1, 1));
    seqs[86002152].emplace_back(SeqLoc(1, 2));

    SmallSeqList read_seqs;
    ReadSmallSeqs(ofilepath, read_seqs);

    ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
  }
  {
    const FilePath& ofilepath = ht_paths[1];

    SmallSeqList seqs;
    seqs[61288890].emplace_back(SeqLoc(2, 0));
    seqs[73645521].emplace_back(SeqLoc(2, 1));
    seqs[86002152].emplace_back(SeqLoc(2, 2));
    seqs[98358783].emplace_back(SeqLoc(2, 3));

    SmallSeqList read_seqs;
    ReadSmallSeqs(ofilepath, read_seqs);

    ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
  }
}

TEST(compare_subseq, test_construct_small_seq_hash_files_3) {
  FilePath filepath = "testdata/test_seq2.txt";

  std::vector<FilePath> ht_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput/");

    ConstructSmallSeqHash(filepath, ht_paths);

    gEnv.setTempFolderPath(saved_temp);
  }

  ASSERT_EQ(1UL, ht_paths.size());

  const FilePath& ofilepath = ht_paths[0];
  ASSERT_TRUE(CheckFileExists(ofilepath.c_str()));

  SmallSeqList seqs;
  seqs[73645521].emplace_back(SeqLoc(0, 0));
  seqs[86002152].emplace_back(SeqLoc(1, 0));
  seqs[98358783].emplace_back(SeqLoc(1, 1));

  SmallSeqList read_seqs;
  ReadSmallSeqs(ofilepath, read_seqs);

  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_construct_small_seq_hash_files_4) {
  FilePath filepath = "testdata/test_seq2.txt";

  std::vector<FilePath> ht_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    uint32_t saved_compare_seq_size = gEnv.getCompareSeqenceSize();

    gEnv.setTempFolderPath("testoutput/");
    gEnv.setCompareSeqenceSize(1);

    ConstructSmallSeqHash(filepath, ht_paths);

    gEnv.setCompareSeqenceSize(saved_compare_seq_size);
    gEnv.setTempFolderPath(saved_temp);
  }

  ASSERT_EQ(2UL, ht_paths.size());

  {
    const FilePath& ofilepath = ht_paths[0];

    SmallSeqList seqs;
    seqs[73645521].emplace_back(SeqLoc(0, 0));

    SmallSeqList read_seqs;
    ReadSmallSeqs(ofilepath, read_seqs);

    ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
  }
  {
    const FilePath& ofilepath = ht_paths[1];

    SmallSeqList seqs;
    seqs[86002152].emplace_back(SeqLoc(1, 0));
    seqs[98358783].emplace_back(SeqLoc(1, 1));

    SmallSeqList read_seqs;
    ReadSmallSeqs(ofilepath, read_seqs);

    ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
  }
}

TEST(compare_subseq, test_compare_small_seq_hash_1) {
  // Prepare input files
  std::vector<FilePath> x_hash_paths;
  std::vector<FilePath> y_hash_paths;

  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput");

    ConstructSmallSeqHash("testdata/test_seq1.txt", x_hash_paths);
    ConstructSmallSeqHash("testdata/test_seq2.txt", y_hash_paths);

    gEnv.setTempFolderPath(saved_temp);
  }

  std::vector<FilePath> compare_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput");
    CompareSmallSeqHash(x_hash_paths, y_hash_paths, compare_paths);
    gEnv.setTempFolderPath(saved_temp);
  }

  ASSERT_EQ(1UL, compare_paths.size());

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  const FilePath& output_path = compare_paths[0];
  std::vector<ComSubseq> com_seqs;
  ReadComSubseqFile(output_path, com_seqs);

  ASSERT_EQ(ans.size(), com_seqs.size());
  std::sort(com_seqs.begin(), com_seqs.end());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, test_compare_small_seq_hash_2) {
  // Prepare input files
  std::vector<FilePath> x_hash_paths;
  std::vector<FilePath> y_hash_paths;

  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    uint32_t saved_compare_seq_size = gEnv.getCompareSeqenceSize();
    gEnv.setTempFolderPath("testoutput");
    gEnv.setCompareSeqenceSize(1);

    ConstructSmallSeqHash("testdata/test_seq1.txt", x_hash_paths);
    ConstructSmallSeqHash("testdata/test_seq2.txt", y_hash_paths);

    gEnv.setCompareSeqenceSize(saved_compare_seq_size);
    gEnv.setTempFolderPath(saved_temp);

    ASSERT_EQ(3UL, x_hash_paths.size());
    ASSERT_EQ(2UL, y_hash_paths.size());
  }

  std::vector<FilePath> compare_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput");
    CompareSmallSeqHash(x_hash_paths, y_hash_paths, compare_paths);
    gEnv.setTempFolderPath(saved_temp);
  }

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  ASSERT_EQ(5UL, compare_paths.size());
  std::vector<ComSubseq> com_seqs;

  for (const auto& f : compare_paths) {
    std::vector<ComSubseq> read_seqs;
    ReadComSubseqFile(f, read_seqs);
    com_seqs.insert(com_seqs.end(), read_seqs.begin(), read_seqs.end());
  }

  ASSERT_EQ(ans.size(), com_seqs.size());
  std::sort(com_seqs.begin(), com_seqs.end());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, test_compare_small_seqs_by_file) {
  std::vector<FilePath> compare_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput");
    CompareSmallSeqs("testdata/test_seq1.txt", "testdata/test_seq2.txt",
                     compare_paths);
    gEnv.setTempFolderPath(saved_temp);
  }

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  ASSERT_EQ(1UL, compare_paths.size());
  std::vector<ComSubseq> com_seqs;

  for (const auto& f : compare_paths) {
    std::vector<ComSubseq> read_seqs;
    ReadComSubseqFile(f, read_seqs);
    com_seqs.insert(com_seqs.end(), read_seqs.begin(), read_seqs.end());
  }

  ASSERT_EQ(ans.size(), com_seqs.size());
  std::sort(com_seqs.begin(), com_seqs.end());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, test_compare_small_seqs_by_file_2) {
  std::vector<FilePath> compare_paths;
  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    uint32_t saved_compare_seq_size = gEnv.getCompareSeqenceSize();
    gEnv.setTempFolderPath("testoutput");
    gEnv.setCompareSeqenceSize(1);

    CompareSmallSeqs("testdata/test_seq1.txt", "testdata/test_seq2.txt",
                     compare_paths);

    gEnv.setCompareSeqenceSize(saved_compare_seq_size);
    gEnv.setTempFolderPath(saved_temp);
  }

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  ASSERT_EQ(5UL, compare_paths.size());
  std::vector<ComSubseq> com_seqs;

  for (const auto& f : compare_paths) {
    std::vector<ComSubseq> read_seqs;
    ReadComSubseqFile(f, read_seqs);
    com_seqs.insert(com_seqs.end(), read_seqs.begin(), read_seqs.end());
  }

  ASSERT_EQ(ans.size(), com_seqs.size());
  std::sort(com_seqs.begin(), com_seqs.end());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

}  // namespace pcpe
