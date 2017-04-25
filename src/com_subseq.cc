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
  infile_(filepath_.c_str(), std::ifstream::in | std::ifstream::binary),
  file_size_(0),
  curr_read_size_(0),
  max_buffer_size_(gEnv.getIOBufferSize() / sizeof(ComSubseq)),
  buffer_(new ComSubseq[(std::size_t)max_buffer_size_]),
  buffer_size_(0),
  buffer_idx_((std::size_t)max_buffer_size_) {

  if (!infile_) {
    LOG_ERROR() << "Open file error - " << filepath_ << std::endl;
    return;
  }

  bool get_status = GetFileSize(filepath_.c_str(), file_size_);
  if (!get_status) {
    LOG_ERROR() << "Get file size error - " << filepath_ << std::endl;
    return;
  }

  readBuffer();
}

void ComSubseqFileReader::readBuffer() {
  if (!infile_.is_open()) {
    LOG_ERROR() << "Read file error. - " << filepath_ << std::endl;
    return;
  }

  if (buffer_idx_ < (std::size_t)buffer_size_) {
    LOG_WARNING() << "Try to refill the buffer when the buffer has data."
                  << std::endl;
    return;
  }

  // fill the buffer with new data
  infile_.read(reinterpret_cast<char*>(buffer_.get()),
      (std::streamsize)sizeof(ComSubseq) * max_buffer_size_);

  infile_.fail();
  std::streamsize read_size = infile_.gcount();

  buffer_size_ = read_size / (std::streamsize)sizeof(ComSubseq);
  buffer_idx_ = 0;

  curr_read_size_ += read_size;

  if (curr_read_size_ > file_size_) {
    LOG_ERROR() << "Read file error. The read bytes (" << curr_read_size_
                << " byte(s)) is over the file size (" << file_size_
                << " byte(s))." << std::endl;
    close();
  }

  if (curr_read_size_ == file_size_)
    close();
}

bool ComSubseqFileReader::readSeq(ComSubseq& seq) {
  if (buffer_idx_ >= (std::size_t)buffer_size_) {
    LOG_ERROR() << "Read sequence error from file `" << filepath_ << "`! "
      << " ridx: " << buffer_idx_
      << ", buffer size: " << buffer_size_ << std::endl;
    return false;
  }

  seq = buffer_[buffer_idx_];
  ++buffer_idx_;

  if (buffer_idx_ >= (std::size_t)buffer_size_ && infile_.is_open())
    readBuffer();

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

  if (file_size % (FileSize)sizeof(ComSubseq) != 0) {
    LOG_ERROR() << "File content error. Please check the file content."
      << std::endl;
    return false;
  }

  std::size_t seq_size =
    static_cast<std::size_t>(file_size / (FileSize)sizeof(ComSubseq));

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
  buffer_(new ComSubseq[gEnv.getIOBufferSize() / sizeof(ComSubseq)]),
  buffer_size_(gEnv.getIOBufferSize() / sizeof(ComSubseq)),
  buffer_idx_(0) {
}

ComSubseqFileWriter::~ComSubseqFileWriter() {
  if (is_open()) {
    close();
  }
}

bool ComSubseqFileWriter::writeSeq(const ComSubseq& seq) {
  if (buffer_idx_ >= (std::size_t)buffer_size_)
    writeBuffer();

  if (buffer_idx_ >= (std::size_t)buffer_size_) {
    LOG_ERROR() << "write sequence error - " << filepath_ << std::endl;
    return false;
  }

  buffer_[buffer_idx_] = seq;
  ++buffer_idx_;

  return true;
}

void ComSubseqFileWriter::writeBuffer() {
  if (!outfile_.is_open()) {
    LOG_ERROR() << "buffer_size_:" << buffer_size_ << std::endl
                << "open write file error" << std::endl;
    return;
  }

  if (buffer_idx_ == 0)
    return;

  outfile_.write(reinterpret_cast<char*>(buffer_.get()),
      (std::streamsize)sizeof(ComSubseq) * (std::streamsize)buffer_idx_);
  buffer_idx_ = 0;
}

bool WriteComSubseqFile(const std::vector<ComSubseq>& com_list,
                        const FilePath& filepath) {
  std::ofstream outfile(filepath, std::ofstream::out | std::ofstream::binary);
  if (!outfile) {
    LOG_ERROR() << "Open a file error - " << filepath << std::endl;
    return false;
  }

  outfile.write(reinterpret_cast<const char*>(com_list.data()),
                std::streamsize(sizeof(ComSubseq) * com_list.size()));
  outfile.close();

  return true;
}

void SplitComSubseqFile(const FilePath& ifilepath,
                        std::vector<FilePath>& ofilepaths) {
  FileSize file_size = 0;
  bool check = GetFileSize(ifilepath.c_str(), file_size);

  if (check == false) {
    LOG_ERROR() << "Get file error. Please check the file exists or not."
                << std::endl;
    return;
  }

  if (file_size == 0)
    return;

  const FileSize buffer_size = gEnv.getBufferSize() /
    sizeof(ComSubseq) * sizeof(ComSubseq);
  if (file_size <= buffer_size) {
    ofilepaths.push_back(ifilepath);
    return;
  }

  // TODO: Resolve the warning
  std::size_t split_files_size = (std::size_t)(file_size / buffer_size);
  if (file_size % buffer_size != 0)
    split_files_size++;

  // Split a file to several files
  std::ifstream infile(ifilepath, std::ifstream::in | std::ifstream::binary);
  std::unique_ptr<ComSubseq[]> buffer(new ComSubseq[
      (std::size_t)buffer_size / sizeof(ComSubseq)]);
  for (std::size_t i = 0; i < split_files_size; ++i) {
    std::size_t read_size =
      (std::size_t)infile.read(reinterpret_cast<char*>(buffer.get()), buffer_size).gcount();

    std::ostringstream oss;
    oss << ifilepath << "_" << i;
    const FilePath ofilepath(oss.str());
    std::ofstream ofile(ofilepath.c_str(),
        std::ofstream::out | std::ofstream::binary);
    ofile.write(reinterpret_cast<const char*>(buffer.get()), (std::streamsize)read_size);
    ofile.close();

    ofilepaths.push_back(ofilepath);
  }

  infile.close();
}

} // namespace pcpe
