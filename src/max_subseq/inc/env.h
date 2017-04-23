#include "pcpe_util.h"

namespace pcpe {

class Env;
extern Env gEnv;

class Env {
 public:
  Env(): io_buffer_size_(16 * 1024 * 1024),    // 16 Mbytes.
         small_seq_length_(6),                 // 6 chars
         temp_folder_("./temp") {
  }

  std::size_t getIOBufferSize() const { return io_buffer_size_; }
  std::size_t getSmallSeqLength() const { return small_seq_length_; }
  const FilePath& getTempFolerPath() const { return temp_folder_; }

  void setIOBufferSize(std::size_t set_size) { io_buffer_size_ = set_size; }
  void setTempFolderPath(const FilePath& path) { temp_folder_ = path; }

 private:
  std::size_t io_buffer_size_;
  const std::size_t small_seq_length_;

  FilePath temp_folder_;
};

} // namespace pcpe
