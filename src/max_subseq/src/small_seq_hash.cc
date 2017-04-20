#include "small_seq_hash.h"

#include <vector>
#include <string>
#include <fstream>

#include "pcpe_util.h"

namespace pcpe {

uint32_t
SmallSeqHashFun::operator() (const SmallSeq& ss) const {
  // The concent of the hash is to consider the seqeucne as a 26-based number.
  // The chars of ithe bio-seqence has 26 variant (A .. Z) so the hash encodes
  // it to a 10-based number. In the concept, each seqsence has an unique
  // hash value. The disadvatance of the hash function is that the size of
  // string can not over than 6 otherwise it would cause overflow of uint32_t.
  const char* s = ss.c_str();

  return (s[0] - 'A') * 1 +         /* 26 ** 0 == 1        */
         (s[1] - 'A') * 26 +        /* 26 ** 1 == 26       */
         (s[2] - 'A') * 676 +       /* 26 ** 2 == 676      */
         (s[3] - 'A') * 17576 +     /* 26 ** 3 == 17576    */
         (s[4] - 'A') * 456976 +    /* 26 ** 4 == 456976   */
         (s[5] - 'A') * 11881376;   /* 26 ** 5 == 11881376 */
}

static
void read_seqence(const FilePath& filepath,
                  SeqList& seqs) {
  std::ifstream in_file(filepath.c_str(), std::ifstream::in);

  std::size_t str_read_size = 0; // the number of seqences of the file.
  in_file >> str_read_size;

  seqs = SeqList(str_read_size);

  // The design of file format is for C. If the program is C, it can read
  // the first argument (`read_size`) and dynamic allocate an array to save
  // it. For convenient, the function uses `std::string` to save the seqence
  // rather than a fixed-size array.
  std::size_t str_length = 0;     // useless, just for backward compatibility
  for (std::size_t i = 0; i < str_read_size; i++)
    in_file >> str_length >> seqs[i];

  in_file.close();
  in_file.clear();
}

static void
construct_smallseqs(const SeqList& seqs,
                    SmallSeqLocList& smallseqs) {
  for (std::size_t sidx = 0; sidx < seqs.size(); ++sidx) {
    // Ignore when the string is less the default size since the value of
    // tiny string is unused in bio research.
    if (seqs[sidx].size() < SmallSeq::default_size())
        continue;

    // Put all fixed-size subseqence with seqeunce index infor to the hash
    // table
    std::size_t end_index = seqs[sidx].size() - SmallSeq::default_size();
    for (std::size_t i = 0; i <= end_index; ++i) {
        auto ss = SmallSeq(seqs[sidx].c_str() + i);
        smallseqs[ss].push_back(SeqLoc(sidx, i));
    }
  }
}

void
read_smallseqs(const FilePath& filepath,
               SmallSeqLocList& smallseqs) {
  // read seq file to a list
  SeqList seqs;
  read_seqence(filepath, seqs);

  // Add small seqence with index and location infor to small seqs.
  construct_smallseqs(seqs, smallseqs);
}

void comsubseq_smallseqs(const FilePath& xfilepath,
                         const FilePath& yfilepath,
                         std::vector<FilePath> rfilepaths) {

}

} // namespace pcpe
