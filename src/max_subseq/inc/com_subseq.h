#pragma once

#include <cstdint>
#include <fstream>
#include <vector>

#include "pcpe_util.h"

namespace pcpe {

static const std::size_t kIOBufferSize = 16 * 1024 * 1024; // 16 MBytes

class ComSubseq {
 public:
  ComSubseq(uint32_t x = 0,
            uint32_t y = 0,
            uint32_t x_loc = 0,
            uint32_t y_loc = 0,
            uint32_t len = 6):
                x_(x), y_(y),
                x_loc_(x_loc), y_loc_(y_loc),
                len_(len) { }

  ComSubseq(const ComSubseq&) = default;
  ComSubseq& operator=(const ComSubseq&) = default;
  ~ComSubseq() = default;

  uint32_t getLength() const { return len_; }
  void setLength(uint32_t len) { len_ = len; }

  bool isContinued(const ComSubseq& rhs) const;

  bool isSameSeqeunce(const ComSubseq& rhs) const;

  bool operator==(const ComSubseq& rhs) const;
  bool operator<(const ComSubseq& rhs) const;
  bool operator>(const ComSubseq& rhs) const;
  bool operator>=(const ComSubseq& rhs) const { return !(*this < rhs); }
  bool operator<=(const ComSubseq& rhs) const { return !(*this > rhs); }
  bool operator!=(const ComSubseq& rhs) const { return !(*this == rhs); }

 private:
    uint32_t x_;
    uint32_t y_;
    uint32_t x_loc_;
    uint32_t y_loc_;
    uint32_t len_;
};


class ComSubseqFileReader{
 public:
  /// Construct with filepath
  ComSubseqFileReader(const Filepath& filepath,
                      std::size_t buffer_size = kIOBufferSize);

  void close() { infile_.close(); }

  /// Read a seq and assign to the parameter.
  void readSeq(ComSubseq& seq);

  /// Get the path of input file
  const char* filepath() const { return filepath_.c_str(); }

  bool is_open() const {
    return infile_.is_open() || read_buffer_idx_ < com_list_size_;
  }

  bool eof() const {
    if (is_open())
      return false;
    else
      return read_buffer_idx_ >= com_list_size_;
  }

  ComSubseqFileReader(const ComSubseqFileReader&) = delete;
  ComSubseqFileReader(const ComSubseqFileReader&&) = delete;
  ComSubseqFileReader& operator=(const ComSubseqFileReader&) = delete;

 private:
  void read_buffer();

  Filepath filepath_;

  std::vector<ComSubseq> com_list_;
  std::size_t com_list_size_;

  std::ifstream infile_;
  std::size_t read_buffer_idx_;

  uint64_t file_size_;               // unit: byte
  std::size_t current_read_file_size_;  // unit: byte
};

/**
 * Read lists of ComSubseq from a file.
 *
 * @param[in] filepath the path of input file
 * @param[out] com_seqs the list of ComSubseqs
 *
 * */
void ReadComSubSeqFile(const Filepath& filepath,
                       std::vector<ComSubseq>& com_seqs);

} // namespace pcpe



