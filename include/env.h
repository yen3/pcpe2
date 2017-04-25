#pragma once

#include <cstdint>

#include "pcpe_util.h"

#include <thread>

namespace pcpe {

class Env;

/// Collect all parameters for the program.
extern Env gEnv;

class Env {
 public:
  Env(): io_buffer_size_(16 * 1024 * 1024),    // 16 Mbytes
         compare_seq_unit_size_(10000),        // 10000 sequences
         buffer_size_(100 * 1024 * 1024),      // 100 Mbytes
         small_seq_length_(6),                 // 6 chars
         thread_size(std::thread::hardware_concurrency()),
         temp_folder_("./temp") {
  }

  uint32_t getIOBufferSize() const { return io_buffer_size_; }
  uint32_t getSmallSeqLength() const { return small_seq_length_; }
  uint32_t getCompareSeqenceSize() const { return compare_seq_unit_size_; }
  uint32_t getBufferSize() const { return buffer_size_; }
  uint32_t getThreadsSize() const { return thread_size; }
  const FilePath& getTempFolerPath() const { return temp_folder_; }

  void setIOBufferSize(uint32_t size) { io_buffer_size_ = size; }
  void setBufferSize(uint32_t size) { buffer_size_ = size; }
  void setTempFolderPath(const FilePath& path) { temp_folder_ = path; }
  void setCompareSeqenceSize(uint32_t size) {
    compare_seq_unit_size_ = size; }
  void setThreadSize(uint32_t size) { thread_size = size; }

 private:
  /// The IO buffer size. The paramemter is used by ComSubseqFileReader/
  /// ComSubseqFileWriter.
  uint32_t io_buffer_size_;

  /// The number of sequences for each comparison and builing hash table.
  uint32_t compare_seq_unit_size_;

  /// The maximum buffer size.
  uint32_t buffer_size_;

  /// The the length of small seq. Don't modify the parameter.
  const uint16_t small_seq_length_;

  /// The number of threads to execute in parallel.
  uint32_t thread_size;

  /// The path to save all temps generated during the programing exectuion.
  FilePath temp_folder_;
};

} // namespace pcpe
