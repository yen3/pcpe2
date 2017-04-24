#pragma once

#include "pcpe_util.h"

#include <thread>

namespace pcpe {

class Env;

/// Collect all parameters for the program.
extern Env gEnv;

class Env {
 public:
  Env(): io_buffer_size_(16 * 1024 * 1024),    // 16 Mbytes.
         compare_seq_unit_size_(10000),        // 10000 sequences
         small_seq_length_(6),                 // 6 chars
         thread_size(std::thread::hardware_concurrency()),
         temp_folder_("./temp") {
  }

  std::size_t getIOBufferSize() const { return io_buffer_size_; }
  std::size_t getSmallSeqLength() const { return small_seq_length_; }
  std::size_t getCompareSeqenceSize() const { return compare_seq_unit_size_; }
  std::size_t getThreadsSize() const { return thread_size; }
  const FilePath& getTempFolerPath() const { return temp_folder_; }

  void setIOBufferSize(std::size_t set_size) { io_buffer_size_ = set_size; }
  void setTempFolderPath(const FilePath& path) { temp_folder_ = path; }
  void setCompareSeqenceSize(std::size_t set_size) {
    compare_seq_unit_size_ = set_size; }
  void setThreadSize(std::size_t set_size) { thread_size = set_size; }

 private:
  /// The IO buffer size. The paramemter is used by ComSubseqFileReader/
  /// ComSubseqFileWriter.
  std::size_t io_buffer_size_;

  /// The number of sequences for each comparison and builing hash table.
  std::size_t compare_seq_unit_size_;

  /// The the length of small seq. Don't modify the parameter.
  const std::size_t small_seq_length_;

  /// The number of threads to execute in parallel.
  std::size_t thread_size;

  /// The path to save all temps generated during the programing exectuion.
  FilePath temp_folder_;
};

} // namespace pcpe
