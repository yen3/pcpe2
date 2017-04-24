#include "com_subseq.h"

#include <fstream>
#include <algorithm>
#include <iostream>

#include "logging.h"
#include "pcpe_util.h"
#include "env.h"

namespace pcpe {

std::ostream& operator<<(std::ostream& out, const ComSubseq& s) {
  out << s.x_ << " " << s.y_ << " "
      << s.x_loc_ << " " << s.y_loc_ << " "
      << s.len_;
  return out;
}

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


ComSubseqFileReader::ComSubseqFileReader(const FilePath& filepath):
  filepath_(filepath),
  com_list_(gEnv.getIOBufferSize() / sizeof(ComSubseq)),
  com_list_size_(0),
  infile_(filepath_.c_str(), std::ifstream::in | std::ifstream::binary),
  read_buffer_idx_(0),
  file_size_(0),
  current_read_file_size_(0) {

  if (!infile_) {
    LOG_ERROR() << "Open file error - " << filepath_ << std::endl;
    return;
  }

  bool get_status = GetFileSize(filepath_.c_str(), file_size_);
  if (!get_status) {
    LOG_ERROR() << "Get file size error - " << filepath_ << std::endl;
    return;
  }

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

bool ComSubseqFileReader::readSeq(ComSubseq& seq) {
  if (read_buffer_idx_ >= com_list_size_) {
    LOG_ERROR() << "Read sequence error from file `" << filepath_ << "`! "
      << " ridx: " << read_buffer_idx_
      << ", buffer size: " << com_list_size_ << std::endl;
    return false;
  }

  seq = com_list_[read_buffer_idx_];
  ++read_buffer_idx_;

  if (read_buffer_idx_ >= com_list_size_ && infile_.is_open())
    read_buffer();

  return true;
}

bool ReadComSubseqFile(const FilePath& filepath,
                       std::vector<ComSubseq>& com_seqs) {
  FileSize file_size = 0;
  if (!GetFileSize(filepath.c_str(), file_size)) {
    LOG_ERROR() << "Chec the file size error - "  << filepath
      << ". Check the file exists or not" << std::endl;
    return false;
  }

  if (file_size % sizeof(ComSubseq) != 0) {
    LOG_ERROR() << "File content error. Please check the file content."
      << std::endl;
    return false;
  }

  std::size_t seq_size =
    static_cast<std::size_t>(file_size / sizeof(ComSubseq));

  // If the file is empty. Ignore the file
  if (seq_size == 0)
    return true;

  com_seqs.resize(seq_size);

  // read the file
  std::ifstream infile(filepath.c_str(),
      std::ifstream::in | std::ifstream::binary);
  infile.read(reinterpret_cast<char*>(com_seqs.data()), file_size);
  infile.close();

  return true;
}

ComSubseqFileWriter::ComSubseqFileWriter(FilePath filepath):
  filepath_(filepath),
  outfile_(filepath_.c_str(), std::ofstream::out | std::ofstream::binary),
  com_list_(gEnv.getIOBufferSize() / sizeof(ComSubseq)),
  com_list_size_(0) {
}

ComSubseqFileWriter::~ComSubseqFileWriter() {
  if (is_open()) {
    close();
  }
}

bool ComSubseqFileWriter::writeSeq(const ComSubseq& seq) {
  if (com_list_size_ >= com_list_.size())
    write_buffer();

  if (com_list_size_ >= com_list_.size()) {
    LOG_ERROR() << "write sequence error - " << filepath_ << std::endl;
    return false;
  }

  com_list_[com_list_size_] = seq;
  ++com_list_size_;

  return true;
}

void ComSubseqFileWriter::write_buffer() {
  if (!outfile_.is_open() || com_list_size_ == 0) {
    if (!outfile_.is_open()) {
      LOG_ERROR() << "com_list_size_:" << com_list_size_ << std::endl
                  << "open write file error" << std::endl;
    }

    return;
  }

  outfile_.write(reinterpret_cast<char*>(com_list_.data()),
      sizeof(ComSubseq) * com_list_size_);
  com_list_size_ = 0;
}


bool WriteComSubseqFile(const std::vector<ComSubseq>& com_list,
                        const FilePath& filepath) {
  std::ofstream outfile(filepath, std::ofstream::out | std::ofstream::binary);
  if (!outfile) {
    LOG_ERROR() << "Open a file error - " << filepath << std::endl;
    return false;
  }

  outfile.write(reinterpret_cast<const char*>(com_list.data()),
                sizeof(ComSubseq) * com_list.size());
  outfile.close();

  return true;
}

} // namespace pcpe

