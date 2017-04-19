#include "com_subseq.h"

namespace pcpe {

bool ComSubseq::isContinued(const ComSubseq& rhs) const {
  return (x_ == rhs.x_) && (y_ == rhs.y_) &&
    ((rhs.x_loc_ - x_loc_ == 1)  || (x_loc_ - rhs.x_loc_ == 1)) &&
    ((rhs.y_loc_ - y_loc_ == 1)  || (y_loc_ - rhs.y_loc_ == 1));
}

bool ComSubseq::isSameSeqeunce(const ComSubseq& rhs) const {
  return (x_ == rhs.x_) && (y_ == rhs.y_);
}

bool ComSubseq::operator==(const ComSubseq& rhs) const {
  return (x_ == rhs.x_) && (y_ == rhs.y_) &&
    (x_loc_ == rhs.x_loc_) && (y_loc_ == rhs.y_loc_);
}

bool ComSubseq::operator<(const ComSubseq& rhs) const {
  if (x_ != rhs.x_)
    return x_ < rhs.x_;

  if (y_ != rhs.y_)
    return y_ < rhs.y_;

  if (x_loc_ != rhs.x_loc_)
    return x_loc_ < rhs.x_loc_;

  return y_loc_ < rhs.y_loc_;
}

bool ComSubseq::operator>(const ComSubseq& rhs) const {
  if (x_ != rhs.x_)
    return x_ > rhs.x_;

  if (y_ != rhs.y_)
    return y_ > rhs.y_;

  if (x_loc_ != rhs.x_loc_)
    return x_loc_ > rhs.x_loc_;

  return y_loc_ > rhs.y_loc_;
}

} // namespace pcpe

