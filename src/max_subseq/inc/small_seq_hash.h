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

void
read_smallseqs(const char* filename,
               SmallSeqLocList& smallseqs);

} // namespace pcpe
