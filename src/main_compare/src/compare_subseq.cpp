#include "compare_subseq.h"

#include <iostream>
#include <fstream>
#include <unordered_set>
#include <utility>


namespace pcpe
{

uint32_t SmallSeqHashFun::operator() (const SmallSeq& ss) const
{
  const char* s = ss.seq_;

  return (s[0] - 'A') * 11881376 + (s[1] - 'A') * 456976 +
         (s[2] - 'A') * 17576 + (s[3] - 'A') * 676 + (s[4] - 'A') * 26 +
         (s[5] - 'A') * 1;
}

static void
read_seqence(const char* filename,
             SeqList& seqs)
{
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
                    SmallSeqLocList& smallseqs)
{
  for (std::size_t sidx = 0; sidx < seqs.size(); ++sidx) {
    if (seqs[sidx].size() < SmallSeq::LENGTH)
        continue;

    std::size_t end_index = seqs[sidx].size() - SmallSeq::LENGTH;
    for (std::size_t i = 0; i <= end_index; ++i) {
        auto ss = SmallSeq(seqs[sidx].c_str() + i);
        smallseqs[ss].push_back(std::make_pair(sidx, i));
    }
  }
}

void
read_smallseqs(const char* filename,
               SmallSeqLocList& smallseqs)
{
  // read seq file to a list
  SeqList seqs;
  read_seqence(filename, seqs);

  // Add small seqence with index and location infor to small seqs.
  construct_smallseqs(seqs, smallseqs);
}

void commom_subseq(const char* seq_fn_a,
                   const char* seq_fn_b)
{
  SmallSeqLocList ssa;
  read_smallseqs(seq_fn_a, ssa);
  std::cout << ssa.size() << std::endl;

  SmallSeqLocList ssb;
  read_smallseqs(seq_fn_b, ssb);
  std::cout << ssb.size() << std::endl;

  std::size_t cs_count = 0;
  for (auto iter = ssa.cbegin(); iter != ssa.cend(); ++iter) {
    if (ssb.count(iter->first) != 0) {
        cs_count++;
    }
  }
  std::cout << cs_count << std::endl;
}

} // namespace pcpe
