#include "max_comsubseq.h"

#include <sstream>
#include <memory>

#include "logging.h"
#include "com_subseq.h"
#include "simple_task.h"
#include "pcpe_util.h"
#include "env.h"

namespace pcpe {

class FindMaxComSubseqTask {
 public:
  FindMaxComSubseqTask(
      const FilePath& input, const FilePath& output):
    ifilepath_(input), ofilepath_(output) {
  }

  void exec();

  const FilePath& getOutput() const { return ofilepath_; }

 private:
   const FilePath& ifilepath_;
   FilePath ofilepath_;
};

void MergeContineousComSubseq(ComSubseq* seqs, bool* merges, std::size_t seqs_size) {
  // Initial the check list.
  std::fill(merges, merges + seqs_size, false);

  // find the maximum common subsequence from seqs
  for (std::size_t base_idx = 0; base_idx < seqs_size; ++base_idx) {
    if (merges[base_idx])
      continue;

    uint32_t base_length = seqs[base_idx].getLength();
    for (std::size_t cidx = base_idx; cidx + 1 < seqs_size; ++cidx) {
      if (seqs[cidx].isContinued(seqs[cidx + 1])) {
        merges[cidx + 1] = true;
        base_length++;
      } else {
        break;
      }
    }
    seqs[base_idx].setLength(base_length);
  }
}

void WriteMergedComSubseqs(ComSubseqFileWriter& writer,
    ComSubseq* seqs, bool* merges, std::size_t seqs_size) {
  for (std::size_t i = 0; i < seqs_size; ++i)
    if (!merges[i])
      writer.writeSeq(seqs[i]);
}

void FindMaxComSubseqTask::exec() {
  FileSize file_size;
  GetFileSize(ifilepath_.c_str(), file_size);

  if (file_size < gEnv.getBufferSize()) {
    // The file size is less than buffer size so it read all seqs and merge
    // them in one time.
    std::vector<ComSubseq> seqs;
    ReadComSubseqFile(ifilepath_, seqs);
    std::unique_ptr<bool[]> merges(new bool[seqs.size()]);

    // Find the maximum common subseqences
    MergeContineousComSubseq(seqs.data(), merges.get(), seqs.size());

    // Write the result
    ComSubseqFileWriter writer(ofilepath_);
    WriteMergedComSubseqs(writer, seqs.data(), merges.get(), seqs.size());
    writer.close();

  } else {
  }
}

static
void CreateFindMaxComSubseqTasks(
    const std::vector<FilePath>& ifilepaths,
    std::vector<std::unique_ptr<FindMaxComSubseqTask>>& tasks) {

  const FilePath& temp_folder = gEnv.getTempFolderPath();

  std::size_t curr_index = 0;
  for (const auto& input : ifilepaths) {
    if (!CheckFileNotEmpty(input.c_str())) {
      LOG_WARNING() << "Get the info of the file error. - "
                    << input << std::endl;
      continue;
    }

    std::ostringstream oss;
    oss << temp_folder << "/max_comsubseq_" << curr_index;
    curr_index++;

    tasks.push_back(std::unique_ptr<FindMaxComSubseqTask>(
          new FindMaxComSubseqTask(input, oss.str())));
  }
}

void MaximumSortedComSubseqs(
    const std::vector<FilePath>& ifilepaths,
    std::vector<FilePath>& ofilepaths) {

  std::vector<std::unique_ptr<FindMaxComSubseqTask>> tasks;

  CreateFindMaxComSubseqTasks(ifilepaths, tasks);

  RunSimpleTasks(tasks);

  for (const auto& task : tasks)
    if (task.get() != nullptr &&
        CheckFileNotEmpty(task->getOutput().c_str()))
      ofilepaths.push_back(task->getOutput().c_str());
}

} // namespace pcpe
