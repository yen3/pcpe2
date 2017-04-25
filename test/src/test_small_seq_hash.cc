#include <gtest/gtest.h>

#include <algorithm>

#include "small_seq_hash.h"
#include "pcpe_util.h"
#include "env.h"

namespace pcpe {

extern uint32_t HashSmallSeq(const char* s);
extern void ReadSequences(const FilePath& filepath,
                          SeqList& seqs);
extern void ConstructSmallSeqs(const SeqList& seqs,
                               std::size_t seqs_begin,
                               std::size_t seqs_end,
                               SmallSeqLocList& smallseqs);
extern void ComapreHashSmallSeqs(const SmallSeqLocList& xs,
                                 const SmallSeqLocList& ys,
                                 const FilePath& ofilepath);


TEST(compare_subseq, test_hash_value) {
    EXPECT_EQ(0UL,   HashSmallSeq("AAAAAA"));
    EXPECT_EQ(1UL,   HashSmallSeq("BAAAAA"));
    EXPECT_EQ(27UL,  HashSmallSeq("BBAAAA"));
    EXPECT_EQ(703UL, HashSmallSeq("BBBAAA"));
}

TEST(compare_subseq, test_read_smallseqs_1) {
  FilePath filepath = "testdata/test_seq1.txt";
  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  SmallSeqLocList seqs;
  ConstructSmallSeqs(seq_list, 0, seq_list.size(), seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 4UL);

  // Chech keys exist
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

  // Chech keys exist
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
  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, CompareSmallSeqs) {
  std::vector<FilePath> files;

  {
    FilePath saved_temp = gEnv.getTempFolerPath();
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
  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], com_seqs[i]);
}

TEST(compare_subseq, CompareSmallSeqs_2) {
  std::vector<FilePath> files;

  {
    FilePath saved_temp = gEnv.getTempFolerPath();
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
  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], com_seqs[i]);
}

} // namespace pcpe
