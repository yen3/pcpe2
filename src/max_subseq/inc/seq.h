#pragma once

#include <string>
#include <vector>
#include <cstdlib>

namespace pcpe {

typedef std::string Seq;
typedef std::vector<Seq> SeqList;

class SmallSeq;

class SmallSeq {
 public:
  // explicit operator const char* () const { return seq_; }

  SmallSeq() { std::memset(seq_, 0, sizeof(char)*MEM_SIZE); }

  explicit SmallSeq(const char* s): SmallSeq() {
    std::memcpy(seq_, s, SEQ_SIZE);
  }

  explicit SmallSeq(const std::string& s): SmallSeq(s.c_str()) { }

  SmallSeq(const SmallSeq& rhs): SmallSeq(rhs.seq_) { }

  const char* c_str() { return seq_; }
  std::size_t size() { return SEQ_SIZE; }
  std::size_t length() { return size(); }

  SmallSeq& operator=(const SmallSeq& rhs) {
    std::memset(seq_, 0, sizeof(char)*MEM_SIZE);
    std::strncpy(seq_, rhs.seq_, SEQ_SIZE);
    return (*this);
  }

  bool operator== (const SmallSeq& rhs) const {
    return std::strcmp(seq_, rhs.seq_) == 0;
  }

  // Convenient function
  bool operator== (const std::string& rhs) const {
    return ((*this) == rhs.c_str());
  }

  // Convenient function
  bool operator== (const char* rhs) const {
    return std::strcmp(seq_, rhs) == 0;
  }

  bool operator>= (const SmallSeq& rhs) const {
    return std::strcmp(seq_, rhs.seq_) > 0;
  }

  bool operator<= (const SmallSeq& rhs) const {
    return std::strcmp(seq_, rhs.seq_) < 0;
  }

  bool operator!= (const SmallSeq& rhs) const { return !((*this) == rhs); }
  bool operator!= (const std::string& rhs) const { return !((*this) == rhs); }
  bool operator!= (const char* rhs) const { return !((*this) == rhs); }
  bool operator> (const SmallSeq& rhs) const { return !((*this) <= rhs); }
  bool operator< (const SmallSeq& rhs) const { return !((*this) >= rhs); }

 private:
  enum { SEQ_SIZE = 6, MEM_SIZE = 7 };

  char seq_[MEM_SIZE];
};

} // namespace pcpe
