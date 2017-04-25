#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include "seq.h"
#include "com_subseq.h"
#include "pcpe_util.h"

#include <cstdint>

namespace pcpe {

static const uint32_t kSmallSeqLength = 6;

struct SeqLoc;

typedef uint32_t SmallSeqHashIndex;
typedef std::vector<SeqLoc> Value;
typedef std::unordered_map<SmallSeqHashIndex, Value> SmallSeqLocList;

struct SeqLoc {
  SeqLoc(): idx(0), loc(0) {}
  SeqLoc(uint32_t pidx, uint32_t ploc):
    idx(pidx), loc(ploc) {}

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
SmallSeqHashIndex HashSmallSeq(const char* s);


/**
 * Find the fix-sized commom subseqences from the two sequence files.
 *
 * @param[in] filepath_x the small-seq hash table
 * @param[in] filepath_y the compared small-seq hash table
 * @param[out] result_filepaths the list of file paths to store the compared
 *                              result.
 *
 * */
void CompareSmallSeqs(const FilePath& filepath_x,
                      const FilePath& filepath_y,
                      std::vector<FilePath>& result_filepaths);

} // namespace pcpe
