#pragma once

#include <cstdint>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "pcpe_util.h"
#include "seq.h"

#include <cstdint>

namespace pcpe {

struct SeqLoc;

using SmallSeqHashIndex = uint32_t;
using Value = std::vector<SeqLoc>;
using SmallSeqLocList = std::unordered_map<SmallSeqHashIndex, Value>;
using SmallSeqs = std::map<SmallSeqHashIndex, Value>;

struct SeqLoc {
  SeqLoc() : idx(0), loc(0) {}
  SeqLoc(uint32_t pidx, uint32_t ploc) : idx(pidx), loc(ploc) {}

  const uint32_t idx;
  const uint32_t loc;
};

class SmallSeqHashFileReader {
 public:
  explicit SmallSeqHashFileReader(const FilePath& filepath);
  ~SmallSeqHashFileReader();

  /// Return true to present a valid read.
  bool readEntry(SmallSeqHashIndex& key, Value& value);
  bool readEntry(std::pair<SmallSeqHashIndex, Value>& entry) {
    return readEntry(entry.first, entry.second);
  }

  /// Get the path of input file
  const FilePath& getPath() { return filepath_; }

  bool is_open();

  bool eof();

  void close();

 private:
  const FilePath filepath_;
  std::ifstream infile_;

  FileSize file_size_;       // unit: byte
  FileSize curr_read_size_;  // unit: byte

  const std::size_t max_buffer_size;  // unit: byte
  std::size_t buffer_size_;           // unit: byte
  std::unique_ptr<uint8_t[]> buffer_;
  std::size_t curr_buffer_idx;
};

class SmallSeqHashFileWriter {
 public:
  explicit SmallSeqHashFileWriter(const FilePath& filepath);
  ~SmallSeqHashFileWriter() { close(); }

  /// Return true to present a valid write.
  bool writeEntry(const SmallSeqHashIndex key, const Value& value);
  bool writeEntry(const std::pair<SmallSeqHashIndex, Value>& entry) {
    return writeEntry(entry.first, entry.second);
  }

  /// Get the path of output file
  const FilePath& getPath() const { return filepath_; }

  void close() {
    writeBuffer();
    outfile_.close();
  }

  bool is_open() { return outfile_.is_open(); }

 private:
  void writeBuffer();

  const FilePath filepath_;
  std::ofstream outfile_;

  const std::size_t max_buffer_size_;  // unit: byte
  std::size_t buffer_size_;            // unit: byte
  std::unique_ptr<uint8_t[]> buffer_;
};

/**
 * Get the hash value of the small seqeuence.
 *
 * The small sequence is protein seqence with 6 chars.  If the length of input
 * is over than 6, it would take the first 6 chars to calculate hash value.
 *
 * @param[in] s The protein seqence.
 *
 * @return an unsigned interger to present hash value.
 * */
constexpr SmallSeqHashIndex HashSmallSeq(const char* s) {
  // The concent of the hash is to consider the seqeucne as a 26-based number.
  // The chars of ithe bio-seqence has 26 variant (A .. Z) so the hash encodes
  // it to a 10-based number. In the concept, each seqsence has an unique
  // hash value. The disadvatance of the hash function is that the size of
  // string can not over than 6 otherwise it would cause overflow of uint32_t.
  return static_cast<SmallSeqHashIndex>(
      (s[0] - 'A') * 1 +        /* 26 ** 0 == 1        */
      (s[1] - 'A') * 26 +       /* 26 ** 1 == 26       */
      (s[2] - 'A') * 676 +      /* 26 ** 2 == 676      */
      (s[3] - 'A') * 17576 +    /* 26 ** 3 == 17576    */
      (s[4] - 'A') * 456976 +   /* 26 ** 4 == 456976   */
      (s[5] - 'A') * 11881376); /* 26 ** 5 == 11881376 */
}

/**
 * Find the fix-sized commom subseqences from the two sequence files.
 *
 * @param[in] filepath_x the small-seq hash table
 * @param[in] filepath_y the compared small-seq hash table
 * @param[out] result_filepaths the list of file paths to store the compared
 *                              result.
 *
 * */
void CompareSmallSeqs(const FilePath& filepath_x, const FilePath& filepath_y,
                      std::vector<FilePath>& result_filepaths);

/**
 * Construct hash tables and save to several files.
 *
 * @param[in] filepath the fasta input file path
 * @param[out] a list of filepaths contain hash value and related data.
 *
 * */
void ConstructSmallSeqHash(const FilePath& filepath,
                           std::vector<FilePath>& hash_filepaths);

/**
 *
 * Find the fix-sized commom subseqences from the two sequence files.
 *
 * @param[in] hash_filepaths_x the small-seq hash tables
 * @param[in] hash_filepaths_y the compared small-seq hash tables
 * @param[out] result_filepaths the list of file paths to store the compared
 *                              result.
 * */
void CompareSmallSeqs(const std::vector<FilePath>& filepath_x,
                      const std::vector<FilePath>& filepath_y,
                      std::vector<FilePath>& result_filepaths);

}  // namespace pcpe
