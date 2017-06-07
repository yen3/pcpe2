#pragma once

#include <cstdint>
#include <unordered_map>
#include <map>
#include <vector>

#include "pcpe_util.h"
#include "seq.h"

#include <cstdint>

namespace pcpe {

static const uint32_t kSmallSeqLength = 6;

struct SeqLoc;

typedef uint32_t SmallSeqHashIndex;
typedef std::vector<SeqLoc> Value;
typedef std::unordered_map<SmallSeqHashIndex, Value> SmallSeqLocList;
typedef std::map<SmallSeqHashIndex, Value> SmallSeqs;

struct SeqLoc {
  SeqLoc() : idx(0), loc(0) {}
  SeqLoc(uint32_t pidx, uint32_t ploc) : idx(pidx), loc(ploc) {}

  const uint32_t idx;
  const uint32_t loc;
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
