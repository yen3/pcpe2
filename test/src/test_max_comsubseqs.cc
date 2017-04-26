#include <gtest/gtest.h>

#include <vector>
#include <utility>
#include <memory>

#include "max_comsubseq.h"
#include "logging.h"
#include "com_subseq.h"
#include "env.h"
#include "pcpe_util.h"

namespace pcpe {

extern void MergeContineousComSubseqs(ComSubseq* seqs,
                                      bool* merges,
                                      std::size_t seqs_size);
extern void WriteMergedComSubseqs(ComSubseqFileWriter& writer,
                                  ComSubseq* seqs,
                                  bool* merges,
                                  std::size_t seqs_size);
extern void MergeComSubseqsFile(const FilePath& ifilepath,
                                const FilePath& ofilepath);

extern std::size_t GetCurrentProcessSize(ComSubseq* seqs,
                                         std::size_t seqs_size);

extern void MergeComSubseqsLargeFile(const FilePath& ifilepath,
                                     const FilePath& ofilepath);

static void CreateTestSeqs(std::vector<ComSubseq>& seqs) {
  seqs.clear();
  seqs.push_back(ComSubseq(0, 0, 1, 0, 6));  // 0:
  seqs.push_back(ComSubseq(1, 0, 1, 0, 6));  // 1:
  seqs.push_back(ComSubseq(1, 1, 2, 0, 6));  // 2:
  seqs.push_back(ComSubseq(2, 0, 1, 0, 6));  // 3:
  seqs.push_back(ComSubseq(2, 1, 2, 0, 6));  // 4: can merge to (2, 1, 2, 0, 7)
  seqs.push_back(ComSubseq(2, 1, 3, 1, 6));  // 5:
  seqs.push_back(ComSubseq(3, 2, 0, 0, 6));  // 6:
  seqs.push_back(ComSubseq(3, 2, 2, 0, 6));  // 7: can merge to (3, 2, 2, 0, 8)
  seqs.push_back(ComSubseq(3, 2, 3, 1, 6));  // 8:
  seqs.push_back(ComSubseq(3, 2, 4, 2, 6));  // 9:
  seqs.push_back(ComSubseq(4, 1, 0, 0, 6));  // 10:
  seqs.push_back(ComSubseq(5, 0, 1, 0, 6));  // 11:
}

static void CreateAnsSeqs(std::vector<ComSubseq>& ans) {
  ans.clear();
  ans.push_back(ComSubseq(2, 1, 2, 0, 7));
  ans.push_back(ComSubseq(3, 2, 2, 0, 8));
}

TEST(max_comsubseqs, MergeContineousComSubseqs) {
  std::vector<ComSubseq> seqs;
  CreateTestSeqs(seqs);

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

TEST(max_comsubseqs, WriteMergedComSubseqs) {
  FilePath ofilepath("./testoutput/test_write_merged_comsubseqs");
  {

    std::vector<ComSubseq> seqs;
    CreateTestSeqs(seqs);
    seqs[4].setLength(7);
    seqs[7].setLength(8);

    bool merges[] = {false, false, false, false,
                     false, true, false, false,
                     true, true, false, false };

    LOG_INFO() << seqs.size() << std::endl;

    ComSubseqFileWriter writer(ofilepath);
    WriteMergedComSubseqs(writer, seqs.data(), merges, seqs.size());
    writer.close();
  }

  std::vector<ComSubseq> ans;
  CreateAnsSeqs(ans);

  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ofilepath, seqs);

  ASSERT_EQ(seqs.size(), ans.size());

  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], seqs[i]);
}

TEST(max_comsubseqs, MergeComSubseqsFile) {
  const FilePath ifilepath("testoutput/test_merged_com_subseqs.in");
  const FilePath ofilepath("testoutput/test_merged_com_subseqs.out");

  {
    std::vector<ComSubseq> seqs;
    CreateTestSeqs(seqs);
    WriteComSubseqFile(seqs, ifilepath);
  }

  MergeComSubseqsFile(ifilepath, ofilepath);
  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ofilepath, seqs);

  std::vector<ComSubseq> ans;
  CreateAnsSeqs(ans);

  ASSERT_EQ(seqs.size(), ans.size());

  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], seqs[i]);
}

TEST(max_comsubseqs, GetCurrentProcessSize) {
  {
    // empty
    std::vector<ComSubseq> seqs;
    ASSERT_EQ(0UL, GetCurrentProcessSize(seqs.data(), seqs.size()));
  }
  {
    // size = 1
    std::vector<ComSubseq> seqs { ComSubseq(0, 0, 1, 0, 6) };
    ASSERT_EQ(1UL, GetCurrentProcessSize(seqs.data(), seqs.size()));
  }
  {
    // All of comsubseq are the same sequences.
    std::vector<ComSubseq> seqs {
       ComSubseq(0, 0, 1, 0, 6),
       ComSubseq(0, 0, 10, 10, 6),
       ComSubseq(0, 0, 20, 20, 6),
    };

    ASSERT_EQ(3UL, GetCurrentProcessSize(seqs.data(), seqs.size()));
  }
  {
    // General case (size = 2)
    std::vector<ComSubseq> seqs;
    CreateAnsSeqs(seqs);

    ASSERT_EQ(1UL, GetCurrentProcessSize(seqs.data(), seqs.size()));
  }
  {
    // General case (size = 13)
    std::vector<ComSubseq> seqs;
    CreateTestSeqs(seqs);

    ASSERT_EQ(seqs.size() - 1,
        GetCurrentProcessSize(seqs.data(), seqs.size()));
  }
}

TEST(max_comsubseqs, MergeComSubseqsLargeFile) {
  const FilePath ifilepath("testoutput/test_merged_com_subseqs.in");
  const FilePath ofilepath("testoutput/test_merged_com_subseqs.out");

  {
    std::vector<ComSubseq> seqs;
    CreateTestSeqs(seqs);
    WriteComSubseqFile(seqs, ifilepath);
  }

  MergeComSubseqsLargeFile(ifilepath, ofilepath);
  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ofilepath, seqs);

  std::vector<ComSubseq> ans;
  CreateAnsSeqs(ans);

  ASSERT_EQ(seqs.size(), ans.size());

  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], seqs[i]);
}

TEST(max_comsubseqs, MergeComSubseqsLargeFile_small_buffer) {
  FilePath ifilepath("./testoutput/test_merged");
  FilePath ofilepath("./testoutput/test_merged.out");

  {
    std::vector<ComSubseq> seqs;
    CreateTestSeqs(seqs);

    WriteComSubseqFile(seqs, ifilepath);

    std::vector<ComSubseq> compare_seqs;
    ReadComSubseqFile(ifilepath, compare_seqs);
  }

  {
    uint32_t saved_buffer_size = gEnv.getBufferSize();
    gEnv.setBufferSize(sizeof(ComSubseq) * 6);
    MergeComSubseqsLargeFile(ifilepath, ofilepath);

    gEnv.setBufferSize(saved_buffer_size);
  }

  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ofilepath, seqs);

  std::vector<ComSubseq> ans;
  CreateAnsSeqs(ans);

  ASSERT_EQ(seqs.size(), ans.size());

  for (std::size_t i = 0; i < ans.size(); ++i)
    ASSERT_EQ(ans[i], seqs[i]);
}

TEST(max_comsubseqs, MaxdSortedComSubseqs) {
  std::vector<FilePath> ifilepaths {
    "./testoutput/test_merged_1", "./testoutput/test_merged_2",
  };
  std::vector<FilePath> ofilepaths;

  {
    std::vector<ComSubseq> seqs;
    CreateTestSeqs(seqs);

    WriteComSubseqFile(seqs, ifilepaths[0]);
    WriteComSubseqFile(seqs, ifilepaths[1]);
  }

  {
    FilePath saved_temp_folder = gEnv.getTempFolderPath();
    gEnv.setTempFolderPath("./testoutput/");

    MaxSortedComSubseqs(ifilepaths, ofilepaths);

    gEnv.setTempFolderPath(saved_temp_folder);
  }

  ASSERT_EQ(ifilepaths.size(), ofilepaths.size());

  std::vector<ComSubseq> ans;
  CreateAnsSeqs(ans);

  for (const auto& filepath : ofilepaths) {
    std::vector<ComSubseq> seqs;
    ReadComSubseqFile(filepath, seqs);

    ASSERT_EQ(seqs.size(), ans.size());
    for (std::size_t i = 0; i < seqs.size(); ++i)
      ASSERT_EQ(seqs[i], ans[i]);
  }
}

} // namespace pcpe
