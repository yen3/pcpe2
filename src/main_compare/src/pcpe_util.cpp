#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include "pcpe_util.h"

namespace pcpe{

void ComSubseqFileReader::readFile(const Filename& fn, std::vector<ComSubseq>& com_list, std::size_t buffer_size){
    std::ifstream infile(fn, std::ifstream::in | std::ifstream::binary);

    std::vector<ComSubseq> buffer(buffer_size);
    while(1){
        infile.read(static_cast<char*>(static_cast<void*>(&buffer[0])), sizeof(ComSubseq)*buffer.size());
        std::size_t read_size = infile.gcount();

        if(infile.eof() && read_size == 0){
            break;
        }

        std::size_t buffer_size = read_size/ sizeof(ComSubseq);
        std::copy(buffer.begin(), buffer.begin() + buffer_size,
                  std::back_inserter(com_list));
    }
    infile.close();
}


void ComSubseqFileWriter::writeFile(const Filename& fn, std::vector<ComSubseq>& com_list, std::size_t buffer_size){
    std::ofstream outfile(fn, std::ofstream::out | std::ofstream::binary);

    std::vector<ComSubseq> buffer(buffer_size);
    for(std::size_t write_size= 0; write_size < com_list.size();){
        auto write_begin = com_list.begin() + write_size;
        auto write_end = (write_size + buffer.size() > com_list.size())?
                              com_list.end():
                              com_list.begin() + write_size + buffer.size();
        auto current_write_size = write_end - write_begin;
        write_size += current_write_size;

        std::copy(write_begin, write_end, buffer.begin());
        outfile.write(static_cast<char*>(static_cast<void*>(&buffer[0])),
                      sizeof(ComSubseq) * current_write_size);
    }
    outfile.flush();
    outfile.close();
}


void ComSubseqFileReader::readSeq(ComSubseq& seq){
    if(read_buffer_idx_ >=  com_list_size_){
        read_buffer();
    }

    if(read_buffer_idx_ >= com_list_size_){
        std::cout << "read sequence error! "
                  << " ridx: " << read_buffer_idx_
                  << ", buffer size: " << com_list_size_ << std::endl;
        return;
    }
    
    seq = com_list_[read_buffer_idx_];
    ++read_buffer_idx_;
}

void ComSubseqFileReader::writeSeq(const ComSubseq& seq){
    return;
}

void ComSubseqFileWriter::readSeq(ComSubseq& seq){
    return;
}

void ComSubseqFileWriter::writeSeq(const ComSubseq& seq){
    if(com_list_size_ >= com_list_.size()){
        write_buffer();
    }

    if(com_list_size_ >= com_list_.size()){
        std::cout << "write sequence error!" << std::endl;
        return;
    }

    com_list_[com_list_size_] = seq;
    ++com_list_size_;
}

void ComSubseqFileReader::read_buffer(){
    if(!infile_.is_open()){
        std::cerr << "Read file error." << std::endl;
        return;
    }

    // move the remaining part to the begining of the buffer
    std::size_t remaining_size = 0;
    if(read_buffer_idx_ < com_list_size_){
        // fidx = from index, remaining_size = to index
        for(std::size_t fidx = read_buffer_idx_;
            fidx < com_list_size_;
            ++fidx, ++remaining_size){
            com_list_[remaining_size] = com_list_[fidx];
        }
    }

    // fill the buffer with new data
    infile_.read(static_cast<char*>(static_cast<void*>(&com_list_[remaining_size])),
                 sizeof(ComSubseq)* (com_list_.size() - remaining_size));

    bool read_fail = infile_.fail();
    std::size_t read_size = infile_.gcount();

    com_list_size_ = remaining_size + read_size / sizeof(ComSubseq);
    read_buffer_idx_ = 0;
    
    if(infile_.eof() || read_fail){
        close();
    }
}

void ComSubseqFileWriter::write_buffer(){
    if(!outfile_.is_open() || com_list_size_ == 0){
        return;
    }

    outfile_.write(static_cast<char*>(static_cast<void*>(&com_list_[0])),
                   sizeof(ComSubseq) * com_list_size_);
    com_list_size_ = 0;
}

} // namespace pcpe

