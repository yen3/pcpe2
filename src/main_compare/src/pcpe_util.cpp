#include "pcpe_util.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <memory>
#include <vector>
#include <string>
#include <algorithm>

#include <unistd.h>
#include <libgen.h>

namespace pcpe {

/**
 * @brief  Get File size
 *
 * @param filename input file name
 *
 * @return the size of the file (unit: byte)
 */
std::size_t get_filesize(const Filename& fn) {
    std::ifstream in(fn, std::ifstream::ate | std::ifstream::binary);
    std::size_t fsize = static_cast<std::size_t>(in.tellg());
    in.close();

    return fsize;
}

bool ComSubseqFileReader::esortMergeCompare(
                         const std::shared_ptr<ComSubseqFileReader> x,
                         const std::shared_ptr<ComSubseqFileReader> y) {
    return x->com_list_[x->read_buffer_idx_] > y->com_list_[y->read_buffer_idx_];
}

/**
 * @brief  Read the whole sequence file
 *
 * @param[in]  fn  read file name
 * @param[out] com_list the seqeunce list. The list could be empty since the
 *                      function use back inserter to read content to the list
 * @param[in]  buffer_size (optional)
 */
void ComSubseqFileReader::readFile(const Filename& fn,
                                   std::vector<ComSubseq>& com_list) {
#if defined(__DEBUG__)
    std::cout << "read file: " << fn << std::endl;
#endif
    std::ifstream infile(fn, std::ifstream::in | std::ifstream::binary);

    std::filebuf* pbuf = infile.rdbuf();
    std::size_t size = pbuf->pubseekoff(0, infile.end, infile.in);
    pbuf->pubseekpos (0, infile.in);

    std::size_t cur_com_list_size = com_list.size();
    std::generate_n(std::back_inserter<std::vector<ComSubseq> >(com_list),
		    size / sizeof(ComSubseq),
		    []() -> ComSubseq { return ComSubseq(); });
    pbuf->sgetn(reinterpret_cast<char*>(&com_list[cur_com_list_size]),size);

    //std::vector<ComSubseq> read_list(size / sizeof(ComSubseq));
    //std::copy(read_list.begin(), read_list.end(),
	      //std::back_inserter(com_list));
}

void ComSubseqFileWriter::writeFile(const Filename& fn,
                                    std::vector<ComSubseq>& com_list) {
#if defined(__DEUBG__)
    std::cout << "write file: " << fn << std::endl;
#endif
    std::ofstream outfile(fn, std::ofstream::out | std::ofstream::binary);
    outfile.write(reinterpret_cast<char*>(&com_list[0]),
		  sizeof(ComSubseq) * com_list.size());
    outfile.close();
}

void ComSubseqFileReader::readSeq(ComSubseq& seq) {
    if (read_buffer_idx_ >= com_list_size_) {
        std::cout << __FILE__ << " " << __LINE__ << ": "
#if defined(__DEBUG__)
                  << fn_ << " "
#endif
                  << "read sequence error! "
                  << " ridx: " << read_buffer_idx_
                  << ", buffer size: " << com_list_size_ << std::endl;
        return;
    }

    seq = com_list_[read_buffer_idx_];
    ++read_buffer_idx_;

    if (read_buffer_idx_ >= com_list_size_ && infile_.is_open()) {
        read_buffer();
    }
}

void ComSubseqFileReader::writeSeq(const ComSubseq& seq) { throw; }

void ComSubseqFileWriter::readSeq(ComSubseq& seq) { throw; }

void ComSubseqFileWriter::writeSeq(const ComSubseq& seq) {
    if (com_list_size_ >= com_list_.size()) {
        write_buffer();
    }

    if (com_list_size_ >= com_list_.size()) {
        std::cerr << __FILE__ << " - " << __FILE__ << ": "
                  << "write sequence error!" << std::endl;
        throw;
    }

    com_list_[com_list_size_] = seq;
    ++com_list_size_;
}

void ComSubseqFileReader::read_buffer() {
    if (!infile_.is_open()) {
        std::cerr << __FILE__ << " - " << __FILE__ << ": "
                  << "Read file error." << std::endl;
        throw;
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

void ComSubseqFileWriter::write_buffer() {
    if (!outfile_.is_open() || com_list_size_ == 0) {
	if (!outfile_.is_open())
	  std::cerr << "com_list_size_:" << com_list_size_ << std::endl
	            << "open write file error" << std::endl;
        return;
    }

    outfile_.write(reinterpret_cast<char*>(&com_list_[0]),
                   sizeof(ComSubseq) * com_list_size_);
    com_list_size_ = 0;
}


void get_pid_filename_prefix(Filename& pid_prefix) {
    pid_t cpid = getpid(); // cpid: current pid

    std::ostringstream os;
    os << cpid;

    pid_prefix = os.str();
}


void get_current_path(Filename& current_path){
    const std::size_t kPwdBufferSize = 1024;
    char p[kPwdBufferSize];

    char* cwd;
    if((cwd = getcwd(p, kPwdBufferSize)) == NULL){
        std::cout << "Get current path error !" << std::endl;
        std::exit(1);
    }

    current_path = cwd;
}


void get_file_basename(const Filename& path, Filename& output_basename) {
    const std::size_t kBaseNameBufferSize = 1024;
    char base_name[kBaseNameBufferSize];
    std::fill(base_name, base_name + path.size() + 1, 0);
    std::copy(path.begin(), path.end(), base_name);

    basename(base_name);
    output_basename = base_name;
}

void get_file_basename_without_sufix(const Filename& path, Filename& output_basename) {
    get_file_basename(path, output_basename);

    if(output_basename.rfind(".") != std::string::npos){
        output_basename = output_basename.substr(0, output_basename.rfind("."));
    }
}


/**
 * @brief
 *
 * @param input_seq_x_fn[in]
 * @param input_seq_y_fn[in]
 * @param output_prefix[out]
 */
void get_common_subseq_output_prefix(const Filename& input_seq_x_fn,
                                     const Filename& input_seq_y_fn,
                                     Filename& output_prefix) {
    Filename cwd;
    get_current_path(cwd);

    Filename pid_prefix;
    get_pid_filename_prefix(pid_prefix);

    Filename basename_x;
    get_file_basename_without_sufix(input_seq_x_fn, basename_x);

    Filename basename_y;
    get_file_basename_without_sufix(input_seq_y_fn, basename_y);

    std::ostringstream os;
    os << cwd << "/" << pid_prefix << "_" << basename_x << "_" << basename_y
       << "_hash_";

    output_prefix = os.str();

#if defined(__DEBUG__)
    std::cout << output_prefix << std::endl;
#endif
}

/**
 * @brief
 *
 * @param input_seq_x_fn[in]
 * @param input_seq_y_fn[in]
 * @param esort_fn[out]
 */
void get_esort_output_file_name(const Filename& input_seq_x_fn,
                                const Filename& input_seq_y_fn,
                                Filename& esort_fn){
    Filename cwd;
    get_current_path(cwd);

    Filename pid_prefix;
    get_pid_filename_prefix(pid_prefix);

    Filename basename_x;
    get_file_basename_without_sufix(input_seq_x_fn, basename_x);

    Filename basename_y;
    get_file_basename_without_sufix(input_seq_y_fn, basename_y);

    std::ostringstream os;
    os << cwd << "/" << pid_prefix << "_" << basename_x << "_" << basename_y
       << "_esort_result.bin";

    esort_fn = os.str();

    std::cout << esort_fn << std::endl;

}


}  // namespace pcpe
