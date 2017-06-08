#include "small_seq_hash.h"

#include <cstdlib>
#include <cstdint>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "com_subseq.h"
#include "env.h"
#include "logging.h"
#include "pcpe_util.h"
#include "simple_task.h"

namespace pcpe {

SmallSeqHashFileWriter::SmallSeqHashFileWriter(const FilePath& filepath)
    : filepath_(filepath),
      outfile_(filepath_.c_str(), std::ofstream::out | std::ofstream::binary),
      max_buffer_size_(gEnv.getIOBufferSize()),
      buffer_size_(0),
      buffer_(new uint8_t[max_buffer_size_]) {}

void SmallSeqHashFileWriter::writeBuffer() {
  if (!is_open() || buffer_size_ == 0) {
    return;
  }

  outfile_.write(reinterpret_cast<const char*>(buffer_.get()),
                 (std::streamsize)(sizeof(uint8_t) * buffer_size_));
  buffer_size_ = 0;
}

bool SmallSeqHashFileWriter::writeEntry(const SmallSeqHashIndex key,
                                        const Value& value) {
  if (value.empty()) return true;

  if (!is_open()) return false;

  if (value.size() > UINT_MAX) {
    // The program does not support so many values in one entry. If the error
    // happens, it means that one entry costs more than 32 GBytes memory.
    //
    // There are two possible results.
    // 1. System has no such memory to run the program.
    // 2. System has enough memory but the program would run very slow.
    //
    // In practice, it's possible to happen but rare. I have no plan to deal
    // with the case currently. Maybe I will try to resolve the problem in
    // future but not now. It's a harder problem for the such scale.
    LOG_FATAL() << "The current file format can not handle the data size."
                << std::endl;
    return false;
  }

  // Get the entry size in the file (unit: byte(s))
  const std::size_t entry_size = sizeof(SmallSeqHashIndex) +
                                 sizeof(uint32_t) +
                                 sizeof(SeqLoc) * value.size();

  if (entry_size > max_buffer_size_) {
    // Deal with the speical case. If the entry size is larger than buffer size,
    // it means the buffer can not contain the entry. One possible soultion is
    // to write the entry to the file directly.

    // The reason to clean the buffer first is to make sure writing sequence
    // order. If users don't mind the order, the action can be ignored.
    writeBuffer();

    // Write index
    outfile_.write(reinterpret_cast<const char*>(&key),
                   sizeof(SmallSeqHashIndex));

    // Write value size
    uint32_t value_size = (uint32_t)value.size();
    outfile_.write(reinterpret_cast<const char*>(&value_size),
                   sizeof(uint32_t));

    // Write value(s)
    outfile_.write(reinterpret_cast<const char*>(value.data()),
                   sizeof(SeqLoc) * value_size);

    return true;
  }

  // In the case, the buffer can handle the entry. The following section does
  // two things.
  // 1. If the currnet empty buffer size can not handle the entry, write the
  //    buffer to the file and clean.
  // 2. Copy the entry to the buffer and update the related information.

  // Check the buffer has enough size to put the entry.
  if (entry_size + buffer_size_ > max_buffer_size_) writeBuffer();

  uint8_t* entry_buffer = buffer_.get() + buffer_size_;

  // Write index
  std::memcpy(entry_buffer, &key, sizeof(SmallSeqHashIndex));
  entry_buffer += sizeof(SmallSeqHashIndex);

  // Write value size
  uint32_t value_size = (uint32_t)value.size();
  std::memcpy(entry_buffer, &value_size, sizeof(uint32_t));
  entry_buffer += sizeof(uint32_t);

  // Write value(s)
  std::memcpy(entry_buffer, value.data(), sizeof(SeqLoc) * value_size);

  // Done to add the entry to the buffer, change the buffer size.
  buffer_size_ += entry_size;

  return true;
}

void ReadSequences(const FilePath& filepath, SeqList& seqs) {
  if (!CheckFileExists(filepath.c_str())) {
    LOG_ERROR() << "The file does not exist - " << filepath << std::endl;
    return;
  }

  std::ifstream in_file(filepath.c_str(), std::ifstream::in);

  std::size_t str_read_size = 0;  // the number of seqences of the file.
  in_file >> str_read_size;

  seqs = SeqList(str_read_size);

  // The design of file format is for C. If the program is C, it can read
  // the first argument (`read_size`) and dynamic allocate an array to save
  // it. For convenient, the function uses `std::string` to save the seqence
  // rather than a fixed-size array.
  std::size_t str_length = 0;  // useless, just for backward compatibility
  for (std::size_t i = 0; i < str_read_size; i++)
    in_file >> str_length >> seqs[i];

  in_file.close();
  in_file.clear();
}

void ConstructSmallSeqs(const SeqList& seqs, std::size_t seqs_begin,
                        std::size_t seqs_end, SmallSeqLocList& smallseqs) {
  constexpr uint32_t noise_hash_index = HashSmallSeq("XXXXXX");

  for (std::size_t sidx = seqs_begin; sidx < seqs_end; ++sidx) {
    // Ignore when the string is less the default size since the value of
    // tiny string is unused in bio research.
    if (seqs[sidx].size() < gEnv.getSmallSeqLength()) continue;

    // Put all fixed-size subseqence with seqeunce index infor to the hash
    // table
    std::size_t end_index = seqs[sidx].size() - gEnv.getSmallSeqLength();
    for (std::size_t i = 0; i <= end_index; ++i) {
      SmallSeqHashIndex index = HashSmallSeq(seqs[sidx].c_str() + i);
      if (index != noise_hash_index)
        smallseqs[index].emplace_back(static_cast<uint32_t>(sidx),
                                      static_cast<uint32_t>(i));
    }
  }
}

void ComapreHashSmallSeqs(const SmallSeqLocList& xs, const SmallSeqLocList& ys,
                          const FilePath& ofilepath) {
  ComSubseqFileWriter writer(ofilepath);

  for (auto xi = xs.cbegin(); xi != xs.cend(); ++xi) {
    auto& s = xi->first;
    auto find_y = ys.find(s);
    if (find_y == ys.end()) continue;

    const Value& xlocs = xi->second;
    const Value& ylocs = find_y->second;

    for (std::size_t xl = 0; xl < xlocs.size(); ++xl) {
      for (std::size_t yl = 0; yl < ylocs.size(); ++yl) {
        ComSubseq css(xlocs[xl].idx, ylocs[yl].idx, xlocs[xl].loc,
                      ylocs[yl].loc, gEnv.getSmallSeqLength());
        writer.writeSeq(css);
      }
    }
  }
  writer.close();

  LOG_INFO() << "Write file done - " << ofilepath << " " << xs.size() << " "
             << ys.size() << std::endl;
}

class CompareSmallSeqTask {
 public:
  CompareSmallSeqTask(const SeqList& xs, const SeqList& ys,
                      std::size_t xs_begin, std::size_t xs_end,
                      std::size_t ys_begin, std::size_t ys_end,
                      const FilePath& output)
      : xs_(xs),
        ys_(ys),
        xs_begin_(xs_begin),
        xs_end_(xs_end),
        ys_begin_(ys_begin),
        ys_end_(ys_end),
        output_(output) {}

  void exec();

  const FilePath& getOutput() { return output_; }

 private:
  const SeqList& xs_;
  const SeqList& ys_;

  const std::size_t xs_begin_;
  const std::size_t xs_end_;

  const std::size_t ys_begin_;
  const std::size_t ys_end_;

  const FilePath output_;
};

void CompareSmallSeqTask::exec() {
  // Construct SmallSeqLocList
  SmallSeqLocList x_smallseqs;
  ConstructSmallSeqs(xs_, xs_begin_, xs_end_, x_smallseqs);

  SmallSeqLocList y_smallseqs;
  ConstructSmallSeqs(ys_, ys_begin_, ys_end_, y_smallseqs);

  // Compare the two lists and save to file.
  ComapreHashSmallSeqs(x_smallseqs, y_smallseqs, output_);
}

void ConstructCompareSmallSeqTasks(
    const SeqList& xs, const SeqList& ys,
    std::vector<std::unique_ptr<CompareSmallSeqTask>>& tasks) {
  const std::size_t kSeqSize = gEnv.getCompareSeqenceSize();

  std::vector<std::size_t> x_steps;
  GetStepsToNumber(xs.size(), kSeqSize, x_steps);

  std::vector<std::size_t> y_steps;
  GetStepsToNumber(ys.size(), kSeqSize, y_steps);

  std::size_t curr_index = 0;
  const FilePath& kTempFolderPrefix = gEnv.getTempFolderPath();
  for (std::size_t x = 0; x < x_steps.size() - 1; ++x) {
    for (std::size_t y = 0; y < y_steps.size() - 1; ++y) {
      // Generate result filename
      std::ostringstream oss;
      oss << kTempFolderPrefix << "/compare_hash_" << curr_index++;
      FilePath output(oss.str());

      tasks.emplace_back(new CompareSmallSeqTask(xs, ys, x_steps[x],
                                                 x_steps[x + 1], y_steps[y],
                                                 y_steps[y + 1], output));
    }
  }

  LOG_INFO() << tasks.size() << " compare small-seq tasks are created."
             << std::endl;
}

void CompareSmallSeqs(const FilePath& xfilepath, const FilePath& yfilepath,
                      std::vector<FilePath>& rfilepaths) {
  // Read sequence from the two files
  SeqList xs;
  ReadSequences(xfilepath, xs);

  SeqList ys;
  ReadSequences(yfilepath, ys);

  LOG_INFO() << "Read sequence done. " << xs.size() << " " << ys.size()
             << std::endl;

  // Construct a task list
  std::vector<std::unique_ptr<CompareSmallSeqTask>> tasks;
  ConstructCompareSmallSeqTasks(xs, ys, tasks);

  // Run all compare tasks
  RunSimpleTasks(tasks);

  // Return the output files
  for (const auto& task : tasks)
    if (task != nullptr && CheckFileExists(task->getOutput().c_str()))
      rfilepaths.push_back(task->getOutput());
}

#if 0
void ConstructSmallSeqHash(const FilePath& filepath,
                           std::vector<FilePath>& hash_filepaths) {
  // Read sequences
  SeqList xs;
  ReadSequences(filepath, xs);

  LOG_INFO() << "Read sequence done. " << xs.size() << " " << std::endl;

  // Construct a task list

  // Construct all hash table files

  // Return the output files
}

void CompareSmallSeqHash(const std::vector<FilePath>& filepath_x,
                         const std::vector<FilePath>& filepath_y,
                         std::vector<FilePath>& result_filepaths) {
  // Construct a task list

  // Return the outputfiles
}

void CompareSmallSeqs2(const FilePath& xfilepath, const FilePath& yfilepath,
                       std::vector<FilePath>& rfilepaths) {
  // Construct hash table for two sequence files.
  std::vector<FilePath> x_hash_paths;
  ConstructSmallSeqHash(xfilepath, x_hash_paths);

  std::vector<FilePath> y_hash_paths;
  ConstructSmallSeqHash(yfilepath, y_hash_paths);

  // Compare the hash tables
  CompareSmallSeqHash(x_hash_paths, y_hash_paths, rfilepaths);
}
#endif

}  // namespace pcpe
