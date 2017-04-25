#pragma once

#include <cstdint>
#include <fstream>
#include <memory>
#include <vector>

#include "pcpe_util.h"

namespace pcpe {

class ComSubseq;
std::ostream& operator<<(std::ostream& out, const ComSubseq& s);

class ComSubseq {
  friend std::ostream& operator<<(std::ostream& out, const ComSubseq& s);
 public:
  ComSubseq() = default;

  ComSubseq(uint32_t x, uint32_t y,
            uint32_t x_loc, uint32_t y_loc,
            uint32_t len):
                x_(x), y_(y), x_loc_(x_loc), y_loc_(y_loc), len_(len) {
  }

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
  friend bool CompareComSubseqFileReaderFirstEntry(
      const ComSubseqFileReader& x,
      const ComSubseqFileReader& y);
 public:
  /// Construct with filepath
  explicit ComSubseqFileReader(const FilePath& filepath);

  void close() { infile_.close(); }

  /// Return ture to present a valid read.
  bool readSeq(ComSubseq& seq);

  /// Get the path of input file
  const char* getFilePath() const { return filepath_.c_str(); }

  bool is_open() const {
    return infile_.is_open() || (std::size_t)buffer_idx_ < (std::size_t)buffer_size_;
  }

  bool eof() const {
    if (is_open())
      return false;
    else
      return (std::size_t)buffer_idx_ >= (std::size_t)buffer_size_;
  }

  ComSubseqFileReader(const ComSubseqFileReader&) = delete;
  ComSubseqFileReader(const ComSubseqFileReader&&) = delete;
  ComSubseqFileReader& operator=(const ComSubseqFileReader&) = delete;

 private:
  void readBuffer();

  FilePath filepath_;
  std::ifstream infile_;

  FileSize file_size_;       // unit: byte
  FileSize curr_read_size_;  // unit: byte

  const std::streamsize max_buffer_size_;
  std::unique_ptr<ComSubseq[]> buffer_;
  std::streamsize buffer_size_;
  std::size_t buffer_idx_;
};

class ComSubseqFileWriter {
 public:
  /// Construct with filepath
  explicit ComSubseqFileWriter(FilePath filepath);
  ~ComSubseqFileWriter();

  /// Return true to present a valid write.
  bool writeSeq(const ComSubseq& seq);

  /// Get the path of output file
  const FilePath& getFilePath() const { return filepath_; }

  void close() {
    writeBuffer();
    outfile_.flush();
    outfile_.close();
  }

  bool is_open() const { return outfile_.is_open(); }

  ComSubseqFileWriter(const ComSubseqFileWriter&) = delete;
  ComSubseqFileWriter(const ComSubseqFileWriter&&) = delete;
  ComSubseqFileWriter& operator=(const ComSubseqFileWriter&) = delete;

 private:
  void writeBuffer();

  FilePath filepath_;
  std::ofstream outfile_;

  std::unique_ptr<ComSubseq[]> buffer_;
  std::streamsize buffer_size_;
  std::size_t buffer_idx_;
};

/**
 * Read lists of ComSubseq from a file.
 *
 * @param[in] filepath the path of input file
 * @param[out] com_seqs the list of ComSubseqs
 *
 * @return true: read file successfully.
 *         false: error happened.
 *
 * */
bool ReadComSubseqFile(const FilePath& filepath,
                       std::vector<ComSubseq>& com_seqs);

/**
 * Write lists of ComSubseq to a file.
 *
 * @param[in] com_seqs the list of ComSubseqs
 * @param[out] filepath the path of output file
 *
 * @return true: read file successfully.
 *         false: error happened.
 *
 * */
bool WriteComSubseqFile(const std::vector<ComSubseq>& com_list,
                        const FilePath& filepath);

/**
 * Split a sequence files to several files. The size of splited files is
 * smaller than or equal the buffer size.
 *
 * @param[in] com_seqs the list of ComSubseqs
 * @param[out] filepath the path of output files
 *
 * */
void SplitComSubseqFile(const FilePath& ifilepath,
                        std::vector<FilePath>& ofilepaths);
} // namespace pcpe



