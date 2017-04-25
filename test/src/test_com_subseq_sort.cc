#include <gtest/gtest.h>

#include <algorithm>

#include "logging.h"
#include "com_subseq.h"
#include "com_subseq_sort.h"
#include "env.h"

namespace pcpe {

TEST(com_subseq_sort, SortComSubseqsFiles) {
  std::vector<FilePath> ifilepaths{"./testoutput/test_esort_file.in"};
  std::vector<FilePath> ofilepaths;

  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("testoutput/");

    SortComSubseqsFiles(ifilepaths, ofilepaths);

    gEnv.setTempFolderPath(saved_temp);
  }

  ASSERT_EQ(ifilepaths.size(), ofilepaths.size());

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ofilepaths[0].c_str(), seqs);

  ASSERT_EQ(seqs.size(), ans.size());
  for (std::size_t i = 0; i < seqs.size(); ++i)
    ASSERT_EQ(seqs[i], ans[i]);

  for (std::size_t i = 0; i < seqs.size() - 1; ++i)
    ASSERT_LE(seqs[i], seqs[i+1]);
}

TEST(com_subseq_sort, SortComSubseqsFiles_small_buffer) {
  std::vector<FilePath> ifilepaths{"./testoutput/test_esort_file.in"};
  std::vector<FilePath> ofilepaths;

  {
    FilePath saved_temp = gEnv.getTempFolderPath();
    std::size_t saved_buffer_size = gEnv.getBufferSize();
    gEnv.setTempFolderPath("testoutput/");
    gEnv.setBufferSize(sizeof(ComSubseq) * 2);

    SortComSubseqsFiles(ifilepaths, ofilepaths);

    gEnv.setTempFolderPath(saved_temp);
    gEnv.setBufferSize(saved_buffer_size);
  }

  ASSERT_EQ(ifilepaths.size(), ofilepaths.size());

  std::vector<ComSubseq> ans;
  ans.push_back(ComSubseq(0, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 0, 1, 0, 6));
  ans.push_back(ComSubseq(1, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 0, 1, 0, 6));
  ans.push_back(ComSubseq(2, 1, 2, 0, 6));
  ans.push_back(ComSubseq(2, 1, 3, 1, 6));

  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ofilepaths[0].c_str(), seqs);

  ASSERT_EQ(seqs.size(), ans.size());
  for (std::size_t i = 0; i < seqs.size(); ++i)
    ASSERT_EQ(seqs[i], ans[i]);

  ASSERT_TRUE(std::is_sorted(seqs.begin(), seqs.end()));
}

} // namespace pcpe

