#include "com_subseq_sort.h"

#include <vector>
#include <sstream>
#include <algorithm>
#include <memory>
#include <queue>

#include "com_subseq.h"
#include "simple_task.h"
#include "logging.h"
#include "env.h"
#include "pcpe_util.h"

namespace pcpe {

bool CompareComSubseqFileReaderFirstEntry(
      const ComSubseqFileReader& x,
      const ComSubseqFileReader& y) {
  return x.com_list_[x.read_buffer_idx_] > y.com_list_[y.read_buffer_idx_];
}

class SortComSubseqsFileTask {
 public:
  SortComSubseqsFileTask(const FilePath& ifilepath,
                         const FilePath& ofilepath):
    ifilepath_(ifilepath), ofilepath_(ofilepath) {
  }
  void exec();

  const FilePath& getOutput() const { return ofilepath_; }

 private:
  void sortSingleFile(const FilePath& ifilepath, const FilePath& ofilepath);
  const FilePath& ifilepath_;
  FilePath ofilepath_;
};

void SplitComSubseqFile(const FilePath& ifilepath,
                        std::vector<FilePath>& ofilepaths) {
  FileSize file_size = 0;
  bool check = GetFileSize(ifilepath.c_str(), file_size);

  if (check == false) {
    LOG_ERROR() << "Get file error. Please check the file exists or not."
                << std::endl;
    return;
  }

  if (file_size == 0)
    return;

  const FileSize buffer_size = gEnv.getBufferSize() /
    sizeof(ComSubseq) * sizeof(ComSubseq);
  if (file_size <= buffer_size) {
    ofilepaths.push_back(ifilepath);
    return;
  }

  std::size_t split_files_size = file_size / buffer_size;
  if (file_size % buffer_size != 0)
    split_files_size++;

  // Split a file to several files
  std::ifstream infile(ifilepath, std::ifstream::in | std::ifstream::binary);
  std::unique_ptr<ComSubseq[]> buffer(new ComSubseq[
      buffer_size / sizeof(ComSubseq)]);
  for (std::size_t i = 0; i < split_files_size; ++i) {
    std::size_t read_size =
      infile.read(reinterpret_cast<char*>(buffer.get()), buffer_size).gcount();

    std::ostringstream oss;
    oss << ifilepath << "_" << i;
    const FilePath ofilepath(oss.str());
    std::ofstream ofile(ofilepath.c_str(),
        std::ofstream::out | std::ofstream::binary);
    ofile.write(reinterpret_cast<const char*>(buffer.get()), read_size);
    ofile.close();

    ofilepaths.push_back(ofilepath);
  }

  infile.close();
}

void SortComSubseqsFileTask::sortSingleFile(
    const FilePath& ifilepath, const FilePath& ofilepath) {
  std::vector<ComSubseq> seqs;
  ReadComSubseqFile(ifilepath, seqs);
  sort(seqs.begin(), seqs.end());
  WriteComSubseqFile(seqs, ofilepath);
}

void SortComSubseqsFileTask::exec() {
  std::vector<FilePath> split_files;
  SplitComSubseqFile(ifilepath_, split_files);

  if (split_files.empty())
    // The input file is empty or error happens.
    return;

  if (split_files.size() == 1) {
    // The size of input file is less than or equal buffer size, just sort these
    // comsubseqs and write to the output file.
    sortSingleFile(split_files[0], ofilepath_);

  } else {
    // The size of input file is more than buffer size. It would do
    // 1. Sort each files.
    // 2. External merge sort for these files.

    // 1. Sort the split files
    for (const auto& filepath : split_files)
      sortSingleFile(filepath, filepath);

    // 2. External sort for the split files

    // Create all readers from split files
    auto cmp_fun = [](const ComSubseqFileReader* x,
                      const ComSubseqFileReader* y) -> bool {
            return CompareComSubseqFileReaderFirstEntry(*x, *y);
        };

    std::priority_queue<ComSubseqFileReader*,
      std::vector<ComSubseqFileReader*>, decltype(cmp_fun)> readers(cmp_fun);

    for (const auto& file : split_files)
      readers.push(new ComSubseqFileReader(file));

    // External merge sort and write the result.
    ComSubseqFileWriter writer(ofilepath_);
    while (!readers.empty()) {
      // Find the minimum entry of these files
      ComSubseqFileReader* reader = readers.top();
      readers.pop();
      ComSubseq seq;
      reader->readSeq(seq);

      // Write the current minimum entry
      writer.writeSeq(seq);

      // If the reader has another entries, push the reader back to the queue.
      if (reader->eof()) {
        reader->close();
        delete reader;
        reader = nullptr;
      } else {
        readers.push(reader);
      }
    }
    writer.close();
  }
}

void ConstructSortComSubseqFileTasks(
    const std::vector<FilePath>& ifilepaths,
    std::vector<SortComSubseqsFileTask*>& tasks) {

  std::size_t curr_index = 0;
  for (const auto& input : ifilepaths) {
    std::ostringstream oss;
    oss << gEnv.getTempFolerPath() << "/sorted_compare_hash_" << curr_index;
    curr_index++;

    SortComSubseqsFileTask* task = new SortComSubseqsFileTask(input, oss.str());
    tasks.push_back(task);
  }
}

void ConstructSortComSubseqFileTasks(
    const std::vector<FilePath>& ifilepaths,
    std::vector<std::unique_ptr<SortComSubseqsFileTask>>& tasks) {

  std::size_t curr_index = 0;
  for (const auto& input : ifilepaths) {
    std::ostringstream oss;
    oss << gEnv.getTempFolerPath() << "/sorted_compare_hash_" << curr_index;
    curr_index++;

    tasks.push_back(std::unique_ptr<SortComSubseqsFileTask>(
            new SortComSubseqsFileTask(input, oss.str())));
  }
}


void SortComSubseqsFiles(const std::vector<FilePath>& ifilepaths,
                         std::vector<FilePath>& ofilepaths) {
  std::vector<std::unique_ptr<SortComSubseqsFileTask>> tasks;
  ConstructSortComSubseqFileTasks(ifilepaths, tasks);

  RunSimpleTasks(tasks);

  for (auto& task : tasks)
    if (task != nullptr && ChechFileExists(task->getOutput().c_str()))
      ofilepaths.push_back(task->getOutput());
}

} // namespace pcpe
