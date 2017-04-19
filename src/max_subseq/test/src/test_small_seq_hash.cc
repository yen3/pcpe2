#include <gtest/gtest.h>

#include "small_seq_hash.h"

namespace pcpe {

TEST(compare_subseq, test_hash_value) {
    SmallSeqHashFun hash_fun;

    EXPECT_EQ(0,   hash_fun(SmallSeq("AAAAAA")));
    EXPECT_EQ(1,   hash_fun(SmallSeq("BAAAAA")));
    EXPECT_EQ(27,  hash_fun(SmallSeq("BBAAAA")));
    EXPECT_EQ(703, hash_fun(SmallSeq("BBBAAA")));
}

TEST(compare_subseq, test_read_smallseqs_1) {
  const char* filename = "testdata/test_seq1.txt";
  SmallSeqLocList seqs;

  read_smallseqs(filename, seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 4);

  // Chech keys exist
  ASSERT_NE(seqs.find(SmallSeq("ABCDEF")), seqs.end());
  ASSERT_NE(seqs.find(SmallSeq("BCDEFG")), seqs.end());
  ASSERT_NE(seqs.find(SmallSeq("CDEFGH")), seqs.end());
  ASSERT_NE(seqs.find(SmallSeq("DEFGHI")), seqs.end());

  // Check all idx& locations are saved
  ASSERT_EQ(seqs[SmallSeq("ABCDEF")].size(), 3);
  ASSERT_EQ(seqs[SmallSeq("BCDEFG")].size(), 3);
  ASSERT_EQ(seqs[SmallSeq("CDEFGH")].size(), 2);
  ASSERT_EQ(seqs[SmallSeq("DEFGHI")].size(), 1);

  {
    Value& idxlocs = seqs[SmallSeq("ABCDEF")];

    ASSERT_EQ(idxlocs.size(), 3);

    ASSERT_EQ(idxlocs[0].idx, 0);
    ASSERT_EQ(idxlocs[0].loc, 0);
    ASSERT_EQ(idxlocs[1].idx, 1);
    ASSERT_EQ(idxlocs[1].loc, 0);
    ASSERT_EQ(idxlocs[2].idx, 2);
    ASSERT_EQ(idxlocs[2].loc, 0);
  }

  {
    Value& idxlocs = seqs[SmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 3);

    ASSERT_EQ(idxlocs[0].idx, 0);
    ASSERT_EQ(idxlocs[0].loc, 1);
    ASSERT_EQ(idxlocs[1].idx, 1);
    ASSERT_EQ(idxlocs[1].loc, 1);
    ASSERT_EQ(idxlocs[2].idx, 2);
    ASSERT_EQ(idxlocs[2].loc, 1);
  }

  {
    Value& idxlocs = seqs[SmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 2);

    ASSERT_EQ(idxlocs[0].idx, 1);
    ASSERT_EQ(idxlocs[0].loc, 2);
    ASSERT_EQ(idxlocs[1].idx, 2);
    ASSERT_EQ(idxlocs[1].loc, 2);
  }

  {
    Value& idxlocs = seqs[SmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 2);
    ASSERT_EQ(idxlocs[0].loc, 3);
  }
}

TEST(compare_subseq, test_read_smallseqs_2) {
  const char* filename = "testdata/test_seq2.txt";
  SmallSeqLocList seqs;

  read_smallseqs(filename, seqs);

  // Check the correct key size
  ASSERT_EQ(seqs.size(), 3);

  // Chech keys exist
  ASSERT_NE(seqs.find(SmallSeq("BCDEFG")), seqs.end());
  ASSERT_NE(seqs.find(SmallSeq("CDEFGH")), seqs.end());
  ASSERT_NE(seqs.find(SmallSeq("DEFGHI")), seqs.end());

  // Check all idx& locations are saved
  ASSERT_EQ(seqs[SmallSeq("BCDEFG")].size(), 1);
  ASSERT_EQ(seqs[SmallSeq("CDEFGH")].size(), 1);
  ASSERT_EQ(seqs[SmallSeq("DEFGHI")].size(), 1);

  {
    Value& idxlocs = seqs[SmallSeq("BCDEFG")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 0);
    ASSERT_EQ(idxlocs[0].loc, 0);
  }

  {
    Value& idxlocs = seqs[SmallSeq("CDEFGH")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 1);
    ASSERT_EQ(idxlocs[0].loc, 0);
  }

  {
    Value& idxlocs = seqs[SmallSeq("DEFGHI")];

    ASSERT_EQ(idxlocs.size(), 1);

    ASSERT_EQ(idxlocs[0].idx, 1);
    ASSERT_EQ(idxlocs[0].loc, 1);
  }
}

} // namespace pcpe
