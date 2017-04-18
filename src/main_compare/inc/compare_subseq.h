#ifndef __COMPARE_COMMOM_SUBSEQENCE__
#define __COMPARE_COMMOM_SUBSEQENCE__

#include <cstdint>
#include <cstring>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>


namespace pcpe
{

typedef std::string Seq;
typedef std::vector<Seq> SeqList;

class SmallSeq;
struct SmallSeqHashFun;

typedef uint32_t SeqIndex;
typedef uint32_t SeqLocation;

typedef std::vector<std::pair<SeqIndex, SeqLocation> > Value;
typedef std::unordered_map<SmallSeq, Value, SmallSeqHashFun> SmallSeqLocList;


class SmallSeq
{
  enum { SEQ_SIZE = 6, MEM_SIZE = 7 };

public:
  friend struct SmallSeqHashFun;

  static const int LENGTH = SEQ_SIZE;

  SmallSeq()
  {
    std::memset(seq_, 0, sizeof(char)*MEM_SIZE);
  }

  SmallSeq(std::string s)
  {
    std::memset(seq_, 0, sizeof(char)*MEM_SIZE);
    std::memcpy(seq_, s.c_str(), SEQ_SIZE);
  }

  SmallSeq(const char* s)
  {
    std::memset(seq_, 0, sizeof(char)*MEM_SIZE);
    std::memcpy(seq_, s, SEQ_SIZE);
  }

  SmallSeq(const SmallSeq& rhs)
  {
    std::memset(seq_, 0, sizeof(char)*MEM_SIZE);
    std::memcpy(seq_, rhs.seq_, SEQ_SIZE);
  }

  SmallSeq& operator=(const SmallSeq& rhs)
  {
    std::memset(seq_, 0, sizeof(char)*MEM_SIZE);
    std::strncpy(seq_, rhs.seq_, SEQ_SIZE);
    return (*this);
  }

  bool operator== (const SmallSeq& rhs) const
  {
    return std::strcmp(seq_, rhs.seq_) == 0;
  }

  bool operator!= (const SmallSeq& rhs) const
  {
    return !((*this) == rhs);
  }

  operator const char * () const
  {
    return seq_;
  }

private:
  char seq_[MEM_SIZE];
};


struct SmallSeqHashFun
{
public:
  uint32_t operator() (const SmallSeq& ss) const ;
};


void
read_smallseqs(const char* filename,
               SmallSeqLocList& smallseqs);

} // namespace pcpe

#endif // __COMPARE_COMMOM_SUBSEQENCE__
