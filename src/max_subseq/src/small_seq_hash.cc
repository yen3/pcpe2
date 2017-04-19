#include "small_seq_hash.h"

#include <fstream>

namespace pcpe {

uint32_t
SmallSeqHashFun::operator() (const SmallSeq& ss) const {
  const char* s = ss.c_str();

  return (s[5] - 'A') * 11881376 +
         (s[4] - 'A') * 456976 +
         (s[3] - 'A') * 17576 +
         (s[2] - 'A') * 676 +
         (s[1] - 'A') * 26 +
         (s[0] - 'A') * 1;
}

static
void read_seqence(const char* filename,
                  SeqList& seqs) {
  std::ifstream in_file(filename, std::ifstream::in);

  std::size_t read_size = 0, str_read_size = 0;
  std::string s;
  in_file >> str_read_size;

  seqs = SeqList(str_read_size);
  for (std::size_t i = 0; i < str_read_size; i++)
    in_file >> read_size >> seqs[i];

  in_file.close();
  in_file.clear();
}

static void
construct_smallseqs(const SeqList& seqs,
                    SmallSeqLocList& smallseqs) {
  for (std::size_t sidx = 0; sidx < seqs.size(); ++sidx) {
    if (seqs[sidx].size() < SmallSeq::default_size())
        continue;

    std::size_t end_index = seqs[sidx].size() - SmallSeq::default_size();
    for (std::size_t i = 0; i <= end_index; ++i) {
        auto ss = SmallSeq(seqs[sidx].c_str() + i);
        smallseqs[ss].push_back(std::make_pair(sidx, i));
    }
  }
}

void
read_smallseqs(const char* filename,
               SmallSeqLocList& smallseqs) {
  // read seq file to a list
  SeqList seqs;
  read_seqence(filename, seqs);

  // Add small seqence with index and location infor to small seqs.
  construct_smallseqs(seqs, smallseqs);
}

} // namespace pcpe
