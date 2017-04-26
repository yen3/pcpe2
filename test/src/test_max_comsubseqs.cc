#include <gtest/gtest.h>

#include <vector>
#include <utility>
#include <memory>

#include "logging.h"
#include "com_subseq.h"
#include "pcpe_util.h"

namespace pcpe {

extern void MergeContineousComSubseqs(ComSubseq* seqs,
                                      bool* merges,
                                      std::size_t seqs_size);

TEST(max_comsubseqs, MergeContineousComSubseqs) {
  std::vector<ComSubseq> seqs{
    ComSubseq(0, 0, 1, 0, 6),
    ComSubseq(1, 0, 1, 0, 6),
    ComSubseq(1, 1, 2, 0, 6),
    ComSubseq(2, 0, 1, 0, 6),   // can merge to (2, 1, 2, 0, 7)
    ComSubseq(2, 1, 2, 0, 6),
    ComSubseq(2, 1, 3, 1, 6),
    ComSubseq(3, 2, 0, 0, 6),
    ComSubseq(3, 2, 2, 0, 6),   // can merge to (3, 2, 2, 0, 8)
    ComSubseq(3, 2, 3, 1, 6),
    ComSubseq(3, 2, 4, 2, 6),
    ComSubseq(4, 1, 0, 0, 6),
    ComSubseq(5, 0, 1, 0, 6)
  };

  std::unique_ptr<bool[]> merges(new bool[seqs.size()]);

  MergeContineousComSubseqs(seqs.data(), merges.get(), seqs.size());

  std::vector<std::pair<ComSubseq, bool>> ans {
    std::make_pair(ComSubseq(0, 0, 1, 0, 6), false),
    std::make_pair(ComSubseq(1, 0, 1, 0, 6), false),
    std::make_pair(ComSubseq(1, 1, 2, 0, 6), false),
    std::make_pair(ComSubseq(2, 0, 1, 0, 6), false),
    std::make_pair(ComSubseq(2, 1, 2, 0, 7), false),
    std::make_pair(ComSubseq(2, 1, 3, 1, 6), true),
    std::make_pair(ComSubseq(3, 2, 0, 0, 6), false),
    std::make_pair(ComSubseq(3, 2, 2, 0, 8), false),
    std::make_pair(ComSubseq(3, 2, 3, 1, 6), true),
    std::make_pair(ComSubseq(3, 2, 4, 2, 6), true),
    std::make_pair(ComSubseq(4, 1, 0, 0, 6), false),
    std::make_pair(ComSubseq(5, 0, 1, 0, 6), false)
  };

  ASSERT_EQ(ans.size(), seqs.size());

  for (std::size_t i = 0; i < ans.size(); ++i) {
    ASSERT_EQ(std::get<0>(ans[i]), seqs[i]) << i;
    ASSERT_EQ(std::get<1>(ans[i]), merges[i]) << i;
  }
}

} // namespace pcpe
