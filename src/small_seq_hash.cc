#include "small_seq_hash.h"

#include <climits>
#include <cstring>
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

const std::size_t kMinimalReadBufferSize =
    sizeof(SmallSeqHashIndex) + sizeof(uint32_t) + sizeof(SeqLoc);

SmallSeqHashFileReader::SmallSeqHashFileReader(const FilePath& filepath)
    : filepath_(filepath),
      infile_(filepath_.c_str(), std::ifstream::in | std::ifstream::binary),
      file_size_(0),
      curr_read_size_(0),
      max_buffer_size_(
          (gEnv.getIOBufferSize() > kMinimalReadBufferSize)
              ? (gEnv.getIOBufferSize() / sizeof(uint32_t) * sizeof(uint32_t))
              : kMinimalReadBufferSize),
      buffer_size_(0),
      buffer_(new uint8_t[max_buffer_size_]),
      used_buffer_size_(0) {
  std::memset(buffer_.get(), 0, sizeof(uint8_t) * max_buffer_size_);

  if (!infile_) {
    LOG_ERROR() << "Open file error - " << filepath_ << std::endl;
    return;
  }

  bool get_status = GetFileSize(filepath_.c_str(), file_size_);
  if (!get_status) {
    LOG_ERROR() << "Get file size error - " << filepath_ << std::endl;
    return;
  }

  readBuffer();
}

void SmallSeqHashFileReader::readBuffer() {
  if (curr_read_size_ >= file_size_ || !infile_.is_open()) {
    return;
  }

  // Move the tail data to the head
  for (std::size_t new_i = 0, i = used_buffer_size_; i < buffer_size_;
       ++i, ++new_i) {
    buffer_[new_i] = buffer_[i];
  }

  // Read new data
  std::size_t remaining_size = buffer_size_ - used_buffer_size_;
  used_buffer_size_ = 0;

  infile_.read(reinterpret_cast<char*>(buffer_.get() + remaining_size),
               static_cast<std::streamsize>(
                   sizeof(uint8_t) * (max_buffer_size_ - remaining_size)));
  infile_.fail();

  // Calculate the total read file size
  FileSize read_size = infile_.gcount();
  curr_read_size_ += read_size;

  buffer_size_ = remaining_size + (std::size_t)read_size;
  if (curr_read_size_ == file_size_) {
    close();
  }

  if (curr_read_size_ > file_size_) {
    LOG_ERROR() << "Read file error. The read bytes (" << curr_read_size_
                << " byte(s)) is over the file size (" << file_size_
                << " byte(s))." << std::endl;
    close();
  }
}

bool SmallSeqHashFileReader::readEntry(SmallSeqHashIndex& key, Value& value) {
  if (!is_open()) {
    LOG_ERROR() << "The file is closed!" << std::endl;
    return false;
  }

  // Check the function can get the key, value size and one value information
  // from the buffer
  if (buffer_size_ - used_buffer_size_ < kMinimalReadBufferSize) {
    readBuffer();
  }

  // Caution: readBuffer function may change the `buffer_size` and
  // `used_buffer_size`.

  // Read the key and value size first.
  uint8_t* curr_buffer = buffer_.get() + used_buffer_size_;
  uint32_t value_size = 0;
  std::memcpy(&key, curr_buffer, sizeof(SmallSeqHashIndex));
  std::memcpy(&value_size, curr_buffer + sizeof(SmallSeqHashIndex),
              sizeof(uint32_t));
  used_buffer_size_ += sizeof(SmallSeqHashIndex) + sizeof(uint32_t);

  if (value_size > UINT_MAX) {
    // The program does not support so many values in one entry, you can get
    // more infor in comments of `SmallSeqHashFileWriter::writeEntry`.
    LOG_FATAL() << "The current program can not handle the data size. "
                << value_size << std::endl;
    return false;
  }

  // Read all values
  value.resize(value_size);
  if (buffer_size_ - used_buffer_size_ >= value.size() * sizeof(SeqLoc)) {
    // Retrieve the value info in one time.
    std::memcpy(value.data(), buffer_.get() + used_buffer_size_,
                sizeof(SeqLoc) * value.size());
    used_buffer_size_ += sizeof(SeqLoc) * value.size();
  } else {
    // Retrieve as many values as possible.
    for (std::size_t curr_value_size = 0; curr_value_size < value.size();) {
      if (buffer_size_ - used_buffer_size_ < sizeof(SeqLoc)) {
        readBuffer();
      }

      std::size_t curr_buffer_size = buffer_size_ - used_buffer_size_;
      std::size_t max_read_value_size = curr_buffer_size / sizeof(SeqLoc);

      // Make sure it's the last time or not.
      if (max_read_value_size + curr_value_size >= value.size()) {
        max_read_value_size = value.size() - curr_value_size;
      }
      std::size_t read_value_size = max_read_value_size * sizeof(SeqLoc);
      std::memcpy(value.data() + curr_value_size,
                  buffer_.get() + used_buffer_size_, read_value_size);

      used_buffer_size_ += read_value_size;
      curr_value_size += max_read_value_size;
    }
  }

  return true;
}

SmallSeqHashFileWriter::SmallSeqHashFileWriter(const FilePath& filepath)
    : filepath_(filepath),
      outfile_(filepath_.c_str(), std::ofstream::out | std::ofstream::binary),
      max_buffer_size_(gEnv.getIOBufferSize() / sizeof(uint32_t) *
                       sizeof(uint32_t)),
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
  const std::size_t entry_size = sizeof(SmallSeqHashIndex) + sizeof(uint32_t) +
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
                        std::size_t seqs_end, SmallSeqList& smallseqs) {
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

class CreateHashTableFileTask {
 public:
  CreateHashTableFileTask(const SeqList& ss, std::size_t ss_begin,
                          std::size_t ss_end, const FilePath& output_path)
      : ss_(ss), ss_begin_(ss_begin), ss_end_(ss_end), output_(output_path) {}
  void exec();

  const FilePath& getOutput() { return output_; }

 private:
  const SeqList& ss_;
  const std::size_t ss_begin_;
  const std::size_t ss_end_;

  FilePath output_;
};

void CreateHashTableFileTask::exec() {
  SmallSeqList small_seqs;
  ConstructSmallSeqs(ss_, ss_begin_, ss_end_, small_seqs);

  SmallSeqHashFileWriter writer(output_);
  for (const auto& entry : small_seqs) {
    writer.writeEntry(entry);
  }
  writer.close();

  LOG_INFO() << "Create hash file: " << output_ << " done." << std::endl;
}

void ConstructHashTableFileTasks(
    const SeqList& ss,
    std::vector<std::unique_ptr<CreateHashTableFileTask>>& tasks) {
  const std::size_t kSeqSize = gEnv.getCompareSeqenceSize();

  std::vector<std::size_t> steps;
  GetStepsToNumber(ss.size(), kSeqSize, steps);
  if (steps.size() <= 1) {
    LOG_ERROR() << "Split seqeuence task error!" << std::endl;
    return;
  }

  static std::size_t curr_index = 0;
  const FilePath& kTempFolderPrefix = gEnv.getTempFolderPath();

  for (std::size_t i = 0; i < steps.size() - 1; ++i) {
    // Generate result filename
    std::ostringstream oss;
    oss << kTempFolderPrefix << "/hash_table_" << curr_index++;
    FilePath output(oss.str());

    tasks.emplace_back(
        new CreateHashTableFileTask(ss, steps[i], steps[i + 1], output));
  }
}

void ConstructSmallSeqHash(const FilePath& filepath,
                           std::vector<FilePath>& hash_filepaths) {
  // Read sequences
  SeqList ss;
  ReadSequences(filepath, ss);

  LOG_INFO() << "Read sequence done. " << ss.size() << " " << std::endl;

  // Construct a task list
  std::vector<std::unique_ptr<CreateHashTableFileTask>> tasks;
  ConstructHashTableFileTasks(ss, tasks);

  // Construct all hash table files
  RunSimpleTasks(tasks);

  // Return the output files
  for (const auto& task : tasks) {
    if (task != nullptr && CheckFileExists(task->getOutput().c_str())) {
      hash_filepaths.emplace_back(task->getOutput());
    } else {
      LOG_WARNING() << "The output file does not exsit: " << task->getOutput()
                    << std::endl;
    }
  }
}

class CompareHashTableFileTask {
 public:
  CompareHashTableFileTask(const FilePath& x_filepath,
                           const FilePath& y_filepath, const FilePath& output)
      : x_filepath_(x_filepath), y_filepath_(y_filepath), output_(output) {}

  void exec();

  FilePath& getOutput() { return output_; }

 private:
  FilePath x_filepath_;
  FilePath y_filepath_;

  FilePath output_;
};

void CompareHashTableFileTask::exec() {
  if (!CheckFileNotEmpty(x_filepath_.c_str()) ||
      !CheckFileNotEmpty(y_filepath_.c_str()))
    return;

  auto write_comsubseq = [](const Value& x_value, const Value& y_value,
                            ComSubseqFileWriter& writer) {
    for (const auto& x : x_value) {
      for (const auto& y : y_value) {
        writer.writeSeq(
            ComSubseq(x.idx, y.idx, x.loc, y.loc, gEnv.getSmallSeqLength()));
      }
    }
  };

  SmallSeqHashFileReader x_reader(x_filepath_);
  SmallSeqHashFileReader y_reader(y_filepath_);

  std::pair<SmallSeqHashIndex, Value> x_entry;
  x_reader.readEntry(x_entry);

  std::pair<SmallSeqHashIndex, Value> y_entry;
  y_reader.readEntry(y_entry);

  ComSubseqFileWriter writer(output_);
  while (!x_reader.eof() || !y_reader.eof()) {
    if (x_entry.first == y_entry.first) {
      write_comsubseq(x_entry.second, y_entry.second, writer);

      if (!x_reader.eof()) x_reader.readEntry(x_entry);
      if (!y_reader.eof()) y_reader.readEntry(y_entry);
    }
    else if (x_reader.eof())                y_reader.readEntry(y_entry);
    else if (y_reader.eof())                x_reader.readEntry(x_entry);
    else if (x_entry.first > y_entry.first) y_reader.readEntry(y_entry);
    else if (x_entry.first < y_entry.first) x_reader.readEntry(x_entry);
  }

  // Deal the last entry
  if (x_entry.first == y_entry.first) {
    write_comsubseq(x_entry.second, y_entry.second, writer);
  }

  x_reader.close();
  y_reader.close();
  writer.close();

  LOG_INFO() << "Compare " << x_filepath_ << " and " << y_filepath_ << " done."
             << std::endl;
}

void ConstructCompareHashTableFileTask(
    const std::vector<FilePath>& x_filepaths,
    const std::vector<FilePath>& y_filepaths,
    std::vector<std::unique_ptr<CompareHashTableFileTask>>& tasks) {
  const FilePath& kTempFolderPrefix = gEnv.getTempFolderPath();
  std::size_t curr_index = 0;
  for (const auto& x : x_filepaths) {
    for (const auto& y : y_filepaths) {
      // Generate result filename
      std::ostringstream oss;
      oss << kTempFolderPrefix << "/compared_hash_" << curr_index++;
      FilePath output(oss.str());

      tasks.emplace_back(new CompareHashTableFileTask(x, y, output));
    }
  }
}

void CompareSmallSeqHash(const std::vector<FilePath>& x_filepaths,
                         const std::vector<FilePath>& y_filepaths,
                         std::vector<FilePath>& result_filepaths) {
  // Construct a task list
  std::vector<std::unique_ptr<CompareHashTableFileTask>> tasks;
  ConstructCompareHashTableFileTask(x_filepaths, y_filepaths, tasks);

  // Run the tasks
  RunSimpleTasks(tasks);

  // Return the outputfiles
  for (const auto& task : tasks)
    if (task != nullptr && CheckFileNotEmpty(task->getOutput().c_str()))
      result_filepaths.emplace_back(task->getOutput());
}

void CompareSmallSeqs(const FilePath& xfilepath,
                      const FilePath& yfilepath,
                      std::vector<FilePath>& rfilepaths) {
  // Construct hash table for two sequence files.
  std::vector<FilePath> x_hash_paths;
  ConstructSmallSeqHash(xfilepath, x_hash_paths);

  std::vector<FilePath> y_hash_paths;
  ConstructSmallSeqHash(yfilepath, y_hash_paths);

  // Compare the hash tables
  CompareSmallSeqHash(x_hash_paths, y_hash_paths, rfilepaths);
}

}  // namespace pcpe
