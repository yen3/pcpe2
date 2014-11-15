#include <vector>
#include <string>
#include <array>
#include <algorithm>

#include "pcpe_util.h"
#include "max_comsubseq.h"

namespace pcpe {

void maximum_common_subseq(const Filename& esort_result,
                           const Filename& reduce_result)
{
    const std::size_t kReadMaxSize = 1000000;
    const std::size_t kWriteBufferSize = 1000000;

    // open the read file
    std::cout << "read file: " << esort_result << std::endl;
    std::ifstream infile(esort_result, std::ifstream::in | std::ifstream::binary);

    // open the reduce output file
    ComSubseqFileWriter outfile(reduce_result, kWriteBufferSize);

    std::size_t remaining_size = 0;
    std::vector<ComSubseq> com_list(kReadMaxSize);
    std::vector<bool> reduced_list(kReadMaxSize);
    std::size_t com_list_size = 0;
    bool read_fail = false;

    while(!infile.eof() && !read_fail){
        std::fill(reduced_list.begin(), reduced_list.end(), false);

        std::cout << "read size " << kReadMaxSize - remaining_size << std::endl;
        // read the file with READ_MAX_SIZE - remaining size 
        infile.read(static_cast<char*>(static_cast<void*>(&com_list[remaining_size])),
                    sizeof(ComSubseq) * (kReadMaxSize - remaining_size));

        // check the read status. if the status is false, it means its the
        // last time to read the file.
        read_fail = infile.fail();

        std::size_t read_size = infile.gcount() / sizeof(ComSubseq);
        com_list_size = remaining_size + read_size;

        // find the lastest index of the part (from READ_MAX_SIZE to 0)
        // if x1 != x2 and y1 != y2
        std::size_t handle_size = com_list_size;
        for(; com_list[handle_size-1].isSameSeqeunce(com_list[handle_size-2]); --handle_size) ;
        if(!read_fail){
            handle_size -= 1;
        }
        std::cout << "handle size " << handle_size << " " << com_list_size << std::endl;

        remaining_size = com_list_size - handle_size;

        // start to reduce
        for(std::size_t i=0; i<handle_size; ++i){
            if(!reduced_list[i]){
                std::size_t l = com_list[i].getLength();
                for(std::size_t j=i; j+1 < handle_size && com_list[j].isContinued(com_list[j+1]); j++){
                    reduced_list[j+1] = true;
                    l++; 
                }

                com_list[i].setLength(l);
            }
        }

        // write the reduce part to file
        for(std::size_t i=0; i<handle_size; ++i){
            if(!reduced_list[i]){
                outfile.writeSeq(com_list[i]);
                if(com_list[i].getLength() >= 8){
                    com_list[i].print();
                }
            }
        }

        // move the remaining to the begin of the vector
        for(std::size_t i=0, r=handle_size;
            r< handle_size+remaining_size;
            ++i, ++r){
            com_list[i] = com_list[r];
        }
    }
    infile.close();
    outfile.close();
}

} // namespace pcpe

