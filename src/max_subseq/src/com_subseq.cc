#include "com_subseq.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#include "logging.h"
#include "pcpe_util.h"

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


ComSubseqFileReader::ComSubseqFileReader(
    const Filepath& filepath, std::size_t buffer_size):
  filepath_(filepath),
  com_list_(buffer_size / sizeof(ComSubseq)),
  com_list_size_(0),
  infile_(filepath_.c_str(), std::ifstream::in | std::ifstream::binary),
  read_buffer_idx_(0),
  file_size_(GetFileSize(filepath_.c_str())),
  current_read_file_size_(0) {

  read_buffer();
}

void ComSubseqFileReader::read_buffer() {
    if (!infile_.is_open()) {
      LOG_ERROR() << "Read file error. - " << filepath_ << std::endl;
      return;
    }

    // move the remaining part to the begining of the buffer
    std::size_t remaining_size = 0;
    if (read_buffer_idx_ < com_list_size_) {
        // fidx = from index, remaining_size = to index
        for (std::size_t fidx = read_buffer_idx_; fidx < com_list_size_;
             ++fidx, ++remaining_size) {
            com_list_[remaining_size] = com_list_[fidx];
        }
    }

    // fill the buffer with new data
    infile_.read(
        reinterpret_cast<char*>(&com_list_[remaining_size]),
        sizeof(ComSubseq) * (com_list_.size() - remaining_size));

    infile_.fail();
    std::size_t read_size = infile_.gcount();

    com_list_size_ = remaining_size + read_size / sizeof(ComSubseq);
    read_buffer_idx_ = 0;

    current_read_file_size_ += read_size;

    if (current_read_file_size_ >= file_size_) {
        close();
    }
}

void ReadComSubSeqFile(const Filepath& filepath,
                       std::vector<ComSubseq>& com_seqs) {
    std::ifstream infile(filepath.c_str(),
        std::ifstream::in | std::ifstream::binary);

    std::filebuf* pbuf = infile.rdbuf();
    std::size_t size = pbuf->pubseekoff(0, infile.end, infile.in);
    pbuf->pubseekpos(0, infile.in);

    std::size_t cur_com_seqs_size = com_seqs.size();
    std::generate_n(std::back_inserter<std::vector<ComSubseq> >(com_seqs),
		    size / sizeof(ComSubseq),
		    []() -> ComSubseq { return ComSubseq(); });
    pbuf->sgetn(reinterpret_cast<char*>(&com_seqs[cur_com_seqs_size]), size);
}

} // namespace pcpe

