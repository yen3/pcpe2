#include <gtest/gtest.h>

#include "small_seq_hash.h"
#include "pcpe_util.h"

namespace pcpe {

extern uint32_t HashSmallSeq(const char* s);
extern void ReadSequences(const FilePath& filepath,
                          SeqList& seqs);
extern void ConstructSmallSeqs(const SeqList& seqs,
                               std::size_t seqs_begin,
                               std::size_t seqs_end,
                               SmallSeqLocList& smallseqs);


TEST(compare_subseq, test_hash_value) {
    EXPECT_EQ(0,   HashSmallSeq("AAAAAA"));
    EXPECT_EQ(1,   HashSmallSeq("BAAAAA"));
    EXPECT_EQ(27,  HashSmallSeq("BBAAAA"));
    EXPECT_EQ(703, HashSmallSeq("BBBAAA"));
}

TEST(compare_subseq, test_read_smallseqs_1) {
  FilePath filepath = "testdata/test_seq1.txt";
  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  SmallSeqLocList seqs;
  ConstructSmallSeqs(seq_list, 0, seq_list.size(), seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 4);

  // Chech keys exist
  ASSERT_NE(seqs.find(HashSmallSeq("ABCDEF")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("BCDEFG")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("CDEFGH")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("DEFGHI")), seqs.end());

  // Check all idx& locations are saved
  ASSERT_EQ(seqs[HashSmallSeq("ABCDEF")].size(), 3);
  ASSERT_EQ(seqs[HashSmallSeq("BCDEFG")].size(), 3);
  ASSERT_EQ(seqs[HashSmallSeq("CDEFGH")].size(), 2);
  ASSERT_EQ(seqs[HashSmallSeq("DEFGHI")].size(), 1);

  {
    Value& idxlocs = seqs[HashSmallSeq("ABCDEF")];

    ASSERT_EQ(idxlocs.size(), 3);

    ASSERT_EQ(idxlocs[0].idx, 0);
    ASSERT_EQ(idxlocs[0].loc, 0);
    ASSERT_EQ(idxlocs[1].idx, 1);
    ASSERT_EQ(idxlocs[1].loc, 0);
    ASSERT_EQ(idxlocs[2].idx, 2);
    ASSERT_EQ(idxlocs[2].loc, 0);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 3);

    ASSERT_EQ(idxlocs[0].idx, 0);
    ASSERT_EQ(idxlocs[0].loc, 1);
    ASSERT_EQ(idxlocs[1].idx, 1);
    ASSERT_EQ(idxlocs[1].loc, 1);
    ASSERT_EQ(idxlocs[2].idx, 2);
    ASSERT_EQ(idxlocs[2].loc, 1);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 2);

    ASSERT_EQ(idxlocs[0].idx, 1);
    ASSERT_EQ(idxlocs[0].loc, 2);
    ASSERT_EQ(idxlocs[1].idx, 2);
    ASSERT_EQ(idxlocs[1].loc, 2);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 2);
    ASSERT_EQ(idxlocs[0].loc, 3);
  }
}

TEST(compare_subseq, test_read_smallseqs_2) {
  FilePath filepath = "testdata/test_seq2.txt";

  SeqList seq_list;
  ReadSequences(filepath, seq_list);

  SmallSeqLocList seqs;
  ConstructSmallSeqs(seq_list, 0, seq_list.size(), seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 3);

  // Chech keys exist
  ASSERT_NE(seqs.find(HashSmallSeq("BCDEFG")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("CDEFGH")), seqs.end());
  ASSERT_NE(seqs.find(HashSmallSeq("DEFGHI")), seqs.end());

  // Check all idx& locations are saved
  ASSERT_EQ(seqs[HashSmallSeq("BCDEFG")].size(), 1);
  ASSERT_EQ(seqs[HashSmallSeq("CDEFGH")].size(), 1);
  ASSERT_EQ(seqs[HashSmallSeq("DEFGHI")].size(), 1);

  {
    Value& idxlocs = seqs[HashSmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 0);
    ASSERT_EQ(idxlocs[0].loc, 0);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 1);
    ASSERT_EQ(idxlocs[0].loc, 0);
  }

  {
    Value& idxlocs = seqs[HashSmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 1);
    ASSERT_EQ(idxlocs[0].loc, 1);
  }
}

} // namespace pcpe
