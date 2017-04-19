#pragma once

#include <cstdint>

namespace pcpe {

class ComSubseq {
 public:
  ComSubseq(uint32_t x = 0,
            uint32_t y = 0,
            uint32_t x_loc = 0,
            uint32_t y_loc = 0,
            uint32_t len = 6):
                x_(x), y_(y),
                x_loc_(x_loc), y_loc_(y_loc),
                len_(len) { }

  ComSubseq(const ComSubseq&) = default;
  ComSubseq& operator=(const ComSubseq&) = default;
  ~ComSubseq() = default;

  uint32_t getLength() const { return len_; }
  void setLength(uint32_t len) { len_ = len; }

  bool isContinued(const ComSubseq& rhs) const;

  bool isSameSeqeunce(const ComSubseq& rhs) const;

  bool operator==(const ComSubseq& rhs) const;
  bool operator<(const ComSubseq& rhs) const;
  bool operator>(const ComSubseq& rhs) const;
  bool operator>=(const ComSubseq& rhs) const { return !(*this < rhs); }
  bool operator<=(const ComSubseq& rhs) const { return !(*this > rhs); }
  bool operator!=(const ComSubseq& rhs) const { return !(*this == rhs); }

 private:
    uint32_t x_;
    uint32_t y_;
    uint32_t x_loc_;
    uint32_t y_loc_;
    uint32_t len_;
};

} // namespace pcpe



