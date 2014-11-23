#include <vector>
#include <string>
#include <algorithm>

#include "pcpe_util.h"
#include "max_comsubseq.h"

namespace pcpe {

/**
 * @brief  
 *
 * @param[inout] com_list
 * @param[in] com_list_size
 * @param[in] last_handle_size
 * @param[out] remaining_size
 */
static void move_remaining_part(ComSubseq* com_list,
                                const std::size_t com_list_size,
                                const std::size_t last_handle_size,
                                std::size_t& remaining_size) {

    remaining_size = com_list_size - last_handle_size;
    for (std::size_t i = 0, r = last_handle_size;
         r < last_handle_size + remaining_size; ++i, ++r) {
        com_list[i] = com_list[r];
    }

}


/**
 * @brief  read file contents to fill the read buffer for maximum_common_subseq
 *
 * @param[in]    infile  input file reader stream
 * @param[in]    remaining_size the current remaining size for the `com_list`
 * @param[in]    read_buffer_size the maximum size for the `com_list`
 * @param[inout] com_list the read buffer, it is also as a output.
 * @param[out]   com_list_size the current size of `com_list` after the
 *                             reading process
 * @param[out]   read_fail the reading status of the infile
 */
static void fill_buffer_from_file(std::ifstream& infile,
                                  const std::size_t remaining_size,
                                  const std::size_t read_buffer_size,
                                  ComSubseq* com_list,
                                  std::size_t& com_list_size,
                                  bool& read_fail) {

        std::cout << "read size " << read_buffer_size - remaining_size
                  << std::endl;

    // read the file with READ_MAX_SIZE - remaining size
    infile.read(reinterpret_cast<char*>(&com_list[remaining_size]),
        sizeof(ComSubseq) * (read_buffer_size - remaining_size));

    // check the read status. if the status is false, it means its the
    // last time to read the file.
    read_fail = infile.fail();

    std::size_t read_size = infile.gcount() / sizeof(ComSubseq);
    com_list_size = remaining_size + read_size;
}

static void cal_current_handle_size(ComSubseq* com_list,
                                    std::size_t com_list_size,
                                    std::size_t handle_size,
                                    bool read_fail) {
    // find the lastest index of the part (from READ_MAX_SIZE to 0)
    // if x1 != x2 and y1 != y2
    handle_size = com_list_size;
    
    // if the read_fail is true, it means that the current reading
    // processing is the latest time, the process has no need to find the
    // complete seqeunce part to reduce.
    if(!read_fail){
        for(; com_list[handle_size - 1].isSameSeqeunce(com_list[handle_size - 2]);
             --handle_size) ;
    }
}


static void reduce_com_seq_list(ComSubseq* com_list, bool* reduced_list,
                                const std::size_t handle_size) {

    std::fill(reduced_list, reduced_list + handle_size, false);
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
}

static void write_reduced_com_list_file(ComSubseqFileWriter& outfile,
                                        ComSubseq* com_list,
                                        bool* reduced_list,
                                        const std::size_t handle_size) {

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
}

void maximum_common_subseq(const Filename& esort_result,
                           const Filename& reduce_result,
                           const std::size_t read_buffer_size,
                           const std::size_t write_buffer_size) {

    // open the read file
    std::ifstream infile(esort_result,
                         std::ifstream::in | std::ifstream::binary);

    // open the reduce output file
    ComSubseqFileWriter outfile(reduce_result, write_buffer_size);

    // create the read buffer
    ComSubseq* com_list = new ComSubseq[read_buffer_size];

    // create the reduced list.
    // the size of reduced list must be the same `com_list`
    bool* reduced_list = new bool[read_buffer_size];

    std::size_t com_list_size = 0;  /**  current reading size of the com list */
    std::size_t handle_size = 0;    /** the size that could be reduce in the current iteration */
                                    /** handle_size <= com_list_size */
    std::size_t remaining_size = 0; /** the unprocessed part of the current com list */
                                    /** remaining_size == com_list_size - handle_size */
    bool read_fail = false;         /** present the read status in the current iterator*/

    while (!infile.eof() && !read_fail) {
        fill_buffer_from_file(infile, remaining_size, read_buffer_size,
                              com_list, com_list_size, read_fail);

        cal_current_handle_size(com_list, com_list_size, handle_size,
                                read_fail);

        // start to reduce
        reduce_com_seq_list(com_list, reduced_list, handle_size);

        // write the reduce part to file
        write_reduced_com_list_file(outfile, com_list, reduced_list,
                                    handle_size);

        // move the remaining to the begin of the vector
        move_remaining_part(com_list, com_list_size, handle_size,
                            remaining_size);
    }
    infile.close();
    outfile.close();

    delete [] com_list;
    delete [] reduced_list;
}

}  // namespace pcpe
