#pragma once

#include <vector>
#include <string>
#include <utility>
#include <unordered_map>

#include "seq.h"

namespace pcpe {

struct SmallSeqHashFun;

typedef uint32_t SeqIndex;
typedef uint32_t SeqLocation;

typedef std::vector<std::pair<SeqIndex, SeqLocation> > Value;
typedef std::unordered_map<SmallSeq, Value, SmallSeqHashFun> SmallSeqLocList;

struct SmallSeqHashFun {
 public:
  uint32_t operator() (const SmallSeq& ss) const;
};

/**
 * Construct a small-seq hash table from a file.
 *
 * @param[in] filepath the path of input file
 * @param[out] smallseqs the small-seq hash table.
 *
 * */
void
read_smallseqs(const char* filepath,
               SmallSeqLocList& smallseqs);

/**
 * Find the fix-sized commom subseqences from the two sequence files.
 *
 * @param[in] filepath_x the small-seq hash table
 * @param[in] filepath_y the compared small-seq hash table
 * @param[out] result_filepaths the list of file paths to store the compared
 *                              result.
 *
 * */
void comsubseq_smallseqs(const char* filepath_x,
                         const char* filepath_y,
                         std::vector<std::string> result_filepaths);

} // namespace pcpe
