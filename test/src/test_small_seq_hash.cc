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
extern void ConstructSmallSeqs(const SeqList& seqs, std::size_t seqs_begin,
                               std::size_t seqs_end,
                               SmallSeqLocList& smallseqs);
extern void ComapreHashSmallSeqs(const SmallSeqLocList& xs,
                                 const SmallSeqLocList& ys,
                                 const FilePath& ofilepath);

bool IsSameSmallSeqs(const SmallSeqs& xs, const SmallSeqs& ys) {
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

void ReadSmallSeqs(const FilePath& ifilepath, SmallSeqs& ss) {
  SmallSeqHashFileReader reader(ifilepath);
  while (!reader.eof()) {
    SmallSeqHashIndex index;
    Value value;
    reader.readEntry(index, value);

    for (const auto& v : value) {
      ss[index].emplace_back(v);
    }
  }
  reader.close();
}

void WriteSmallSeqs(const SmallSeqs& ss, const FilePath& ofilepath) {
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

TEST(compare_subseq, test_read_smallseqs_1) {
  FilePath filepath = "testdata/test_seq1.txt";
  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  SmallSeqLocList seqs;
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
    Value& idxlocs = seqs[HashSmallSeq("ABCDEF")];

    ASSERT_EQ(idxlocs.size(), 3UL);

    ASSERT_EQ(idxlocs[0].idx, 0U);
    ASSERT_EQ(idxlocs[0].loc, 0U);
    ASSERT_EQ(idxlocs[1].idx, 1U);
    ASSERT_EQ(idxlocs[1].loc, 0U);
    ASSERT_EQ(idxlocs[2].idx, 2U);
    ASSERT_EQ(idxlocs[2].loc, 0U);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 3UL);

    ASSERT_EQ(idxlocs[0].idx, 0U);
    ASSERT_EQ(idxlocs[0].loc, 1U);
    ASSERT_EQ(idxlocs[1].idx, 1U);
    ASSERT_EQ(idxlocs[1].loc, 1U);
    ASSERT_EQ(idxlocs[2].idx, 2U);
    ASSERT_EQ(idxlocs[2].loc, 1U);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 2UL);

    ASSERT_EQ(idxlocs[0].idx, 1U);
    ASSERT_EQ(idxlocs[0].loc, 2U);
    ASSERT_EQ(idxlocs[1].idx, 2U);
    ASSERT_EQ(idxlocs[1].loc, 2U);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 2U);
    ASSERT_EQ(idxlocs[0].loc, 3U);
  }
}

TEST(compare_subseq, test_read_smallseqs_2) {
  FilePath filepath = "testdata/test_seq2.txt";

  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  SmallSeqLocList seqs;
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
    Value& idxlocs = seqs[HashSmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 0U);
    ASSERT_EQ(idxlocs[0].loc, 0U);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 1U);
    ASSERT_EQ(idxlocs[0].loc, 0U);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1UL);

    ASSERT_EQ(idxlocs[0].idx, 1U);
    ASSERT_EQ(idxlocs[0].loc, 1U);
  }
}

TEST(compare_subseq, CompareHashSmallSeqs) {
  SmallSeqLocList x_seqs;
  SmallSeqLocList y_seqs;

  {
    FilePath filepath = "testdata/test_seq1.txt";
    SeqList seq_list;
    ReadSequences(filepath, seq_list);

    SmallSeqLocList seqs;
    ConstructSmallSeqs(seq_list, 0, seq_list.size(), x_seqs);
  }
  {
    FilePath filepath = "testdata/test_seq2.txt";
    SeqList seq_list;
    ReadSequences(filepath, seq_list);

    SmallSeqLocList seqs;
    ConstructSmallSeqs(seq_list, 0, seq_list.size(), y_seqs);
  }

  FilePath out_filepath = "testoutput/compare_com_subseqs.bin";
  ComapreHashSmallSeqs(x_seqs, y_seqs, out_filepath);

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  std::vector<ComSubseq> com_seqs;
  ReadComSubseqFile(out_filepath, com_seqs);
  std::sort(com_seqs.begin(), com_seqs.end());

  ASSERT_EQ(ans.size(), com_seqs.size());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, CompareSmallSeqs) {
  std::vector<FilePath> files;

  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput/");
    CompareSmallSeqs("testdata/test_seq1.txt", "testdata/test_seq2.txt", files);
    gEnv.setTempFolderPath(saved_temp);
  }

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  ASSERT_EQ(files.size(), 1UL);

  std::vector<ComSubseq> com_seqs;
  ReadComSubseqFile(files[0], com_seqs);
  std::sort(com_seqs.begin(), com_seqs.end());

  ASSERT_EQ(ans.size(), com_seqs.size());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, CompareSmallSeqs_2) {
  std::vector<FilePath> files;

  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    std::size_t saved_compare_seq_size = gEnv.getCompareSeqenceSize();

    gEnv.setTempFolderPath("testoutput/");
    gEnv.setCompareSeqenceSize(1);

    CompareSmallSeqs("testdata/test_seq1.txt", "testdata/test_seq2.txt", files);

    gEnv.setTempFolderPath(saved_temp);
    gEnv.setCompareSeqenceSize(saved_compare_seq_size);
  }

  ASSERT_EQ(files.size(), 6UL);

  std::vector<ComSubseq> com_seqs;
  for (const auto& f : files) {
    std::vector<ComSubseq> read_seqs;
    ReadComSubseqFile(f, read_seqs);
    com_seqs.insert(com_seqs.end(), read_seqs.begin(), read_seqs.end());
  }
  std::sort(com_seqs.begin(), com_seqs.end());

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  ASSERT_EQ(ans.size(), com_seqs.size());
  for (std::size_t i = 0; i < ans.size(); ++i) ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, test_small_hash_table_reader) {
  SmallSeqs seqs;
  seqs[0].emplace_back(SeqLoc(1, 0));
  seqs[2].emplace_back(SeqLoc(2, 0));
  seqs[2].emplace_back(SeqLoc(5, 0));
  seqs[3].emplace_back(SeqLoc(3, 0));
  seqs[4].emplace_back(SeqLoc(0, 0));

  const FilePath input_path = "testdata/test_small_hash_table_reader_1.in";

  SmallSeqs read_seqs;
  ReadSmallSeqs(input_path, read_seqs);

  ASSERT_TRUE(IsSameSmallSeqs(seqs, read_seqs));
}

TEST(compare_subseq, test_small_hash_table_reader_small_buffer) {
  InitLogging(LoggingLevel::kDebug);

  SmallSeqs seqs;
  seqs[0].emplace_back(SeqLoc(1, 0));
  seqs[2].emplace_back(SeqLoc(2, 0));
  seqs[2].emplace_back(SeqLoc(5, 0));
  seqs[3].emplace_back(SeqLoc(3, 0));
  seqs[4].emplace_back(SeqLoc(0, 0));

  const FilePath input_path = "testdata/test_small_hash_table_reader_1.in";

  SmallSeqs read_seqs;
  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    gEnv.setIOBufferSize(48);

    ReadSmallSeqs(input_path, read_seqs);

    gEnv.setIOBufferSize(savedIOBufferSize);
  }

  ASSERT_TRUE(IsSameSmallSeqs(seqs, read_seqs));
}

TEST(compare_subseq, test_small_hash_table_reader_minimal_buffer) {
  InitLogging(LoggingLevel::kDebug);

  SmallSeqs seqs;
  seqs[0].emplace_back(SeqLoc(1, 0));
  seqs[2].emplace_back(SeqLoc(2, 0));
  seqs[2].emplace_back(SeqLoc(5, 0));
  seqs[3].emplace_back(SeqLoc(3, 0));
  seqs[4].emplace_back(SeqLoc(0, 0));

  const FilePath input_path = "testdata/test_small_hash_table_reader_1.in";

  SmallSeqs read_seqs;
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
  SmallSeqs seqs;
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
  SmallSeqs read_seqs;
  ReadSmallSeqs(output_path, read_seqs);
  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_small_hash_table_writer_small_buffer) {
  // Read the seqs
  SmallSeqs seqs;
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
  SmallSeqs read_seqs;
  ReadSmallSeqs(output_path, read_seqs);
  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

TEST(compare_subseq, test_small_hash_table_writer_minimal_buffer) {
  // Read the seqs
  SmallSeqs seqs;
  ReadSmallSeqs("testdata/test_small_hash_table_reader_1.in", seqs);

  // Write the sequences
  const FilePath output_path = "testoutput/test_small_hash_table_writer";
  {
    std::size_t savedIOBufferSize = gEnv.getIOBufferSize();
    //gEnv.setIOBufferSize(sizeof(SmallSeqHashIndex) + sizeof(uint32_t) +
                         //sizeof(SeqLoc));

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
  SmallSeqs read_seqs;
  ReadSmallSeqs(output_path, read_seqs);
  ASSERT_TRUE(IsSameSmallSeqs(read_seqs, seqs));
}

}  // namespace pcpe
