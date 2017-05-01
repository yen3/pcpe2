#include "max_comsubseq.h"

#include <sstream>
#include <memory>

#include "logging.h"
#include "com_subseq.h"
#include "simple_task.h"
#include "pcpe_util.h"
#include "env.h"

namespace pcpe {

void MergeContineousComSubseqs(ComSubseq* seqs,
                               bool* merges,
                               std::size_t seqs_size) {
  // Initial the check list.
  std::fill(merges, merges + seqs_size, false);

  // Find the maximum common subsequence from seqs
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

/**
 * Remove all ComSubseqs are merged.
 *
 * @return the new seqs_size.
 * */
std::size_t RemoveMergedComSubseqs(ComSubseq* seqs,
                                   bool* merges,
                                   std::size_t seqs_size) {

  std::size_t new_seqs_size = 0;

  // cidx = copy index
  for (std::size_t cidx = 0; cidx < seqs_size; ++cidx) {
    if (!merges[cidx]) {
      seqs[new_seqs_size] = seqs[cidx];
      new_seqs_size++;
    }
  }

  // Reset merge state
  std::fill(merges, merges + new_seqs_size, false);

  return new_seqs_size;
}

void WriteMergedComSubseqs(ComSubseqFileWriter& writer,
                           ComSubseq* seqs,
                           bool* merges,
                           std::size_t seqs_size) {
  const std::size_t min_output_length = gEnv.getMinimumOutputLength();

  for (std::size_t i = 0; i < seqs_size; ++i)
    if (!merges[i] && seqs[i].getLength() >= min_output_length)
      writer.writeSeq(seqs[i]);
}

void MergeComSubseqsFile(const FilePath& ifilepath,
                         const FilePath& ofilepath) {
  // Create read buffer and merges
  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ifilepath, seqs);
  std::unique_ptr<bool[]> merges(new bool[seqs.size()]);

  // Find the maximum common subseqences
  MergeContineousComSubseqs(seqs.data(), merges.get(), seqs.size());

  // Write the result
  ComSubseqFileWriter writer(ofilepath);
  WriteMergedComSubseqs(writer, seqs.data(), merges.get(), seqs.size());
  writer.close();
}

std::size_t GetCurrentProcessSize(ComSubseq* seqs, std::size_t seqs_size) {
  // find the lastest index of the part (from seq_size to 0)
  // if x1 != x2 and y1 != y2
  if (seqs_size <= 1)
    return seqs_size;

  for (std::size_t pidx = seqs_size - 1; pidx != 0; --pidx)
    if (!seqs[pidx].isSameSeqeunce(seqs[pidx - 1]))
      return pidx;

  // The whole buffer is the same two sequences.
  return seqs_size;
}

void MergeComSubseqsLargeFile(const FilePath& ifilepath,
                              const FilePath& ofilepath) {
  // Create the read buffer and check list.
  const std::size_t max_seqs_size = gEnv.getBufferSize() / sizeof(ComSubseq);
  std::unique_ptr<ComSubseq[]> seqs(new ComSubseq[max_seqs_size]);
  std::unique_ptr<bool[]> merges(new bool[max_seqs_size]);

  std::ifstream ifile(ifilepath.c_str(),
      std::ifstream::in | std::ifstream::binary);
  ComSubseqFileWriter writer(ofilepath);

  FileSize file_size;
  GetFileSize(ifilepath.c_str(), file_size);

  std::streamoff read_file_size = 0;
  std::size_t unprocess_seqs_size = 0;

  while (read_file_size < file_size) {
    // Read file to Fill the buffer
    ifile.read(reinterpret_cast<char*>(&(seqs.get()[unprocess_seqs_size])),
        static_cast<std::streamsize>((max_seqs_size - unprocess_seqs_size) *
          sizeof(ComSubseq)));

    read_file_size += ifile.gcount();
    std::size_t read_seqs_size = (std::size_t)ifile.gcount() /
      sizeof(ComSubseq);
    std::size_t seqs_size = unprocess_seqs_size + read_seqs_size;

    // Find the seqences can be processed.
    std::size_t process_seqs_size;
    bool compressed_mode = false;
    if (read_file_size >= file_size) {
      // It's the last time to merge and write.
      process_seqs_size = seqs_size;
    } else {
      process_seqs_size = GetCurrentProcessSize(seqs.get(), seqs_size);

      if (process_seqs_size == seqs_size && seqs_size >= 2) {
        // In the situation, the whole buffer is the same two sequences.
        // the process_seqs_size is seqs_size. It means can not find the
        // correct process size. One solution is to merge the comsubseqs in
        // the buffer and remain the last entry for next reading and merging.
        compressed_mode = true;
      }
    }

    // Find the maximum common subseqences
    MergeContineousComSubseqs(seqs.get(), merges.get(), process_seqs_size);

    // Speical case: If all sequences in the buffer are the same, remove all
    // merged comsubseqs and remaing the last entry.
    if (compressed_mode) {
      seqs_size = RemoveMergedComSubseqs(seqs.get(), merges.get(),
          process_seqs_size);
      process_seqs_size = seqs_size - 1;
    }

    // Write the result
    WriteMergedComSubseqs(writer, seqs.get(), merges.get(), process_seqs_size);

    // Move unprocessed seqs to the begin of the buffer.
    unprocess_seqs_size = seqs_size - process_seqs_size;
    if (unprocess_seqs_size != 0)
      std::copy(seqs.get() + process_seqs_size, seqs.get() + seqs_size,
                seqs.get());
  }

  ifile.close();
  writer.close();
}

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

void FindMaxComSubseqTask::exec() {
  FileSize file_size;
  GetFileSize(ifilepath_.c_str(), file_size);

  if (file_size < gEnv.getBufferSize()) {
    // The file size is less than buffer size so it reads all seqs and merge
    // them in one time.
    MergeComSubseqsFile(ifilepath_, ofilepath_);
  } else {
    // The file size is more than buffer size so it reads a buffer size file to
    // process and write the result.
    MergeComSubseqsLargeFile(ifilepath_, ofilepath_);
  }

  LOG_INFO() << "Find max common subseqences - "<< ofilepath_  << std::endl;
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

    tasks.emplace_back(new FindMaxComSubseqTask(input, oss.str()));
  }
}

void MaxSortedComSubseqs(const std::vector<FilePath>& ifilepaths,
                         std::vector<FilePath>& ofilepaths) {

  std::vector<std::unique_ptr<FindMaxComSubseqTask>> tasks;
  CreateFindMaxComSubseqTasks(ifilepaths, tasks);

  RunSimpleTasks(tasks);

  for (const auto& task : tasks)
    if (task.get() != nullptr && CheckFileNotEmpty(task->getOutput().c_str()))
      ofilepaths.push_back(task->getOutput().c_str());
}

} // namespace pcpe
