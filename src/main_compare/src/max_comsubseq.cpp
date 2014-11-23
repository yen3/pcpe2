#include <vector>
#include <string>
#include <algorithm>

#include "pcpe_util.h"
#include "max_comsubseq.h"

namespace pcpe {

void maximum_common_subseq(const Filename& esort_result,
                           const Filename& reduce_result,
                           const std::size_t read_buffer_size,
                           const std::size_t write_buffer_size) {
    // open the read file
    std::cout << "read file: " << esort_result << std::endl;
    std::ifstream infile(esort_result,
                         std::ifstream::in | std::ifstream::binary);

    // open the reduce output file
    ComSubseqFileWriter outfile(reduce_result, write_buffer_size);

    std::size_t remaining_size = 0;
    ComSubseq* com_list = new ComSubseq[read_buffer_size];
    bool* reduced_list = new bool[read_buffer_size];

    std::size_t com_list_size = 0;
    bool read_fail = false;

    while (!infile.eof() && !read_fail) {
        std::fill(reduced_list, reduced_list + read_buffer_size, false);

        std::cout << "read size " << read_buffer_size - remaining_size << std::endl;
        // read the file with READ_MAX_SIZE - remaining size
        infile.read(
            reinterpret_cast<char*>(&com_list[remaining_size]),
            sizeof(ComSubseq) * (read_buffer_size - remaining_size));

        // check the read status. if the status is false, it means its the
        // last time to read the file.
        read_fail = infile.fail();

        std::size_t read_size = infile.gcount() / sizeof(ComSubseq);
        com_list_size = remaining_size + read_size;

        // find the lastest index of the part (from READ_MAX_SIZE to 0)
        // if x1 != x2 and y1 != y2
        std::size_t handle_size = com_list_size;
        for (; com_list[handle_size - 1].isSameSeqeunce(
                 com_list[handle_size - 2]);
             --handle_size)
            ;
        if (!read_fail) {
            handle_size -= 1;
        }
        std::cout << "handle size " << handle_size << " " << com_list_size
                  << std::endl;

        remaining_size = com_list_size - handle_size;

        // start to reduce
        for (std::size_t i = 0; i < handle_size; ++i) {
            if (!reduced_list[i]) {
                std::size_t l = com_list[i].getLength();
                for (std::size_t j = i;
                     j + 1 < handle_size &&
                         com_list[j].isContinued(com_list[j + 1]);
                     j++) {
                    reduced_list[j + 1] = true;
                    l++;
                }

                com_list[i].setLength(l);
            }
        }

        // write the reduce part to file
        for (std::size_t i = 0; i < handle_size; ++i) {
            if (!reduced_list[i]) {
                outfile.writeSeq(com_list[i]);
#if defined(__DEBUG__)
                if (com_list[i].getLength() >= 8) {
                    com_list[i].print();
                }
#endif
            }
        }

        // move the remaining to the begin of the vector
        for (std::size_t i = 0, r = handle_size;
             r < handle_size + remaining_size; ++i, ++r) {
            com_list[i] = com_list[r];
        }
    }
    infile.close();
    outfile.close();

    delete [] com_list;
    delete [] reduced_list;
}

}  // namespace pcpe
