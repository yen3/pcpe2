#include <gtest/gtest.h>

#include <array>
#include <vector>
#include "pcpe_util.h"
#include "max_comsubseq.h"


namespace pcpe {
    
/*****************************************************************************/
// Global const definition 
/*****************************************************************************/
const std::size_t kBufferSize = 1000;  


/*****************************************************************************/
// Typedef 
/*****************************************************************************/
typedef std::array<ComSubseq, kBufferSize> ComSubseqTestBuffer;
typedef std::array<bool, kBufferSize> ComSubseqTestReducedBuffer;


/*****************************************************************************/
// Test function protypes
/*****************************************************************************/
void assert_comsubseq_list(const ComSubseqList& cslx,
                           const ComSubseqList& csly);
extern void move_remaining_part(ComSubseq* com_list,
                                const std::size_t com_list_size,
                                const std::size_t last_handle_size,
                                std::size_t& remaining_size);

extern void fill_buffer_from_file(std::ifstream& infile,
                                  const std::size_t remaining_size,
                                  const std::size_t read_buffer_size,
                                  ComSubseq* com_list,
                                  std::size_t& com_list_size, bool& read_fail);

extern void cal_current_handle_size(ComSubseq* com_list,
                                    std::size_t com_list_size,
                                    bool read_fail,
                                    std::size_t& handle_size);

extern void reduce_com_seq_list(ComSubseq* com_list, bool* reduced_list,
                                const std::size_t handle_size) ;

extern void write_reduced_com_list_file(ComSubseqFileWriter& outfile,
                                        ComSubseq* com_list, bool* reduced_list,
                                        const std::size_t handle_size);

extern void maximum_common_subseq(const Filename& esort_result,
                                  const Filename& reduce_result,
                                  const std::size_t read_buffer_size,
                                  const std::size_t write_buffer_size);


/*****************************************************************************/
// Test functions
/*****************************************************************************/
TEST(max_comsubseq, test_move_remaining_part) {
    ComSubseqTestBuffer com_list;

    com_list[com_list.size() - 2] = ComSubseq(1, 2, 3, 4);
    com_list[com_list.size() - 1] = ComSubseq(5, 6, 7, 8);

    com_list[0] = ComSubseq(0, 0, 0, 0);
    com_list[1] = ComSubseq(1, 1, 1, 1);

    std::size_t remaining_size = 0;
    move_remaining_part(com_list.data(), com_list.size(), com_list.size() - 2,
                        remaining_size);

    EXPECT_EQ(remaining_size, 2);
    EXPECT_EQ(com_list[0], ComSubseq(1, 2, 3, 4));
    EXPECT_EQ(com_list[1], ComSubseq(5, 6, 7, 8));
}


TEST(max_comsubseq, test_cal_current_handle_size) {
    ComSubseqTestBuffer com_list;

    com_list[0] = ComSubseq(0, 0, 1, 0);
    com_list[1] = ComSubseq(1, 0, 1, 0);
    com_list[2] = ComSubseq(1, 1, 2, 0);
    com_list[3] = ComSubseq(2, 0, 1, 0);
    com_list[4] = ComSubseq(2, 1, 2, 0);
    com_list[5] = ComSubseq(2, 1, 3, 1);

    std::size_t com_list_size = 6;

    std::size_t handle_size = 0;
    cal_current_handle_size(com_list.data(), com_list_size, false, handle_size);
    EXPECT_EQ(handle_size, 4);

    cal_current_handle_size(com_list.data(), com_list_size, true, handle_size);
    EXPECT_EQ(handle_size, 6);

    cal_current_handle_size(com_list.data(), com_list_size - 1, false,
                            handle_size);
    EXPECT_EQ(handle_size, 4);

    cal_current_handle_size(com_list.data(), com_list_size - 1, true,
                            handle_size);
    EXPECT_EQ(handle_size, 5);
}

TEST(max_comsubseq, test_reduce_com_seq_list){
    ComSubseqTestBuffer com_list;
    ComSubseqTestReducedBuffer reduced_list;

    com_list[0] = ComSubseq(0, 0, 1, 0);
    com_list[1] = ComSubseq(1, 0, 1, 0);
    com_list[2] = ComSubseq(1, 1, 2, 0);
    com_list[3] = ComSubseq(2, 0, 1, 0);
    com_list[4] = ComSubseq(2, 1, 2, 0);
    com_list[5] = ComSubseq(2, 1, 3, 1);
    com_list[6] = ComSubseq(2, 1, 4, 2);

    std::size_t handle_size = 4;
    reduce_com_seq_list(com_list.data(), reduced_list.data(), handle_size);

    EXPECT_FALSE(reduced_list[0]);
    EXPECT_FALSE(reduced_list[1]);
    EXPECT_FALSE(reduced_list[2]);
    EXPECT_FALSE(reduced_list[3]);

    handle_size = 7;
    reduce_com_seq_list(com_list.data(), reduced_list.data(), handle_size);

    EXPECT_FALSE(reduced_list[0]);
    EXPECT_FALSE(reduced_list[1]);
    EXPECT_FALSE(reduced_list[2]);
    EXPECT_FALSE(reduced_list[3]);
    EXPECT_FALSE(reduced_list[4]);
    EXPECT_TRUE(reduced_list[5]);
    EXPECT_TRUE(reduced_list[6]);

    EXPECT_EQ(com_list[4].getLength(), 8);

    // next test case
    com_list[0] = ComSubseq(0, 0, 1, 0);
    com_list[1] = ComSubseq(1, 0, 1, 0);
    com_list[2] = ComSubseq(1, 0, 2, 1);
    com_list[3] = ComSubseq(1, 1, 2, 0);
    com_list[4] = ComSubseq(2, 0, 1, 0);
    com_list[5] = ComSubseq(2, 1, 2, 0);
    com_list[6] = ComSubseq(2, 1, 3, 1);
    com_list[7] = ComSubseq(2, 1, 4, 2);

    handle_size = 8;

    reduce_com_seq_list(com_list.data(), reduced_list.data(), handle_size);
    std::vector<bool> ans_reduced = {false, false, true, false,
                                     false, false, true, true};
    for (std::size_t i = 0; i < ans_reduced.size(); ++i) {
        EXPECT_EQ(reduced_list[i], ans_reduced[i]);
    }

    std::vector<std::size_t> ans_length = {6, 7, 6, 6, 6, 8, 6, 6};
    for (std::size_t i = 0; i < ans_length.size(); ++i) {
        EXPECT_EQ(com_list[i].getLength(), ans_length[i]);
    }
}


TEST(max_comsubseq, test_fill_buffer_from_file){
    ComSubseqTestBuffer com_list;

    com_list[0] = ComSubseq(0, 0, 1, 0);
    com_list[1] = ComSubseq(1, 0, 1, 0);
    std::size_t remaining_size = 2;

    std::size_t com_list_size = 0; 
    bool read_fail = false; 

    Filename ifn("./testdata/test_esort_file.in"); /* input file name */
    std::ifstream in_file(ifn.c_str(),
                          std::ifstream::in | std::ifstream::binary);

    fill_buffer_from_file(in_file, remaining_size, com_list.size(),
                          com_list.data(), com_list_size, read_fail);

    EXPECT_EQ(com_list_size, 6 + remaining_size);
    EXPECT_EQ(read_fail, true);

    std::vector<ComSubseq> ans_list = {
         ComSubseq(0, 0, 1, 0),
         ComSubseq(1, 0, 1, 0),
         ComSubseq(1, 1, 2, 0),
         ComSubseq(2, 1, 2, 0),
         ComSubseq(0, 0, 1, 0),
         ComSubseq(1, 0, 1, 0),
         ComSubseq(2, 0, 1, 0),
         ComSubseq(2, 1, 3, 1)
    };

    for(std::size_t aidx = 0; aidx < ans_list.size(); ++aidx){
        EXPECT_EQ(ans_list[aidx], com_list[aidx]);
    }
}

TEST(max_comsubseq, test_write_reduced_com_list_file){
    {
        ComSubseqTestBuffer com_list;
        ComSubseqTestReducedBuffer reduced_list;

        com_list[0] = ComSubseq(0, 0, 1, 0);
        com_list[1] = ComSubseq(1, 0, 1, 0);
        com_list[2] = ComSubseq(1, 0, 2, 1);
        com_list[3] = ComSubseq(1, 1, 2, 0);
        com_list[4] = ComSubseq(2, 0, 1, 0);
        com_list[5] = ComSubseq(2, 1, 2, 0);
        com_list[6] = ComSubseq(2, 1, 3, 1);
        com_list[7] = ComSubseq(2, 1, 4, 2);

        std::size_t handle_size = 8;

        reduce_com_seq_list(com_list.data(), reduced_list.data(), handle_size);
        std::vector<bool> ans_reduced = {false, false, true, false,
                                         false, false, true, true};
        for (std::size_t i = 0; i < ans_reduced.size(); ++i) {
            EXPECT_EQ(reduced_list[i], ans_reduced[i]);
        }

        std::vector<std::size_t> ans_length = {6, 7, 6, 6, 6, 8, 6, 6};
        for (std::size_t i = 0; i < ans_length.size(); ++i) {
            EXPECT_EQ(com_list[i].getLength(), ans_length[i]);
        }

        std::string ofn("./testoutput/test_write_reduced_com_list_file.out");
        ComSubseqFileWriter csfw(ofn);
        write_reduced_com_list_file(csfw, com_list.data(), reduced_list.data(),
                                    handle_size);
    }

    std::vector<ComSubseq> com_seq_list;
    std::string ifn("./testoutput/test_write_reduced_com_list_file.out");
    ComSubseqFileReader::readFile(ifn, com_seq_list);

    std::vector<ComSubseq> ans_list = {
        ComSubseq(0, 0, 1, 0, 6),
        ComSubseq(1, 0, 1, 0, 7),
        ComSubseq(1, 1, 2, 0, 6),
        ComSubseq(2, 0, 1, 0, 6),
        ComSubseq(2, 1, 2, 0, 8),
    };
    assert_comsubseq_list(com_seq_list, ans_list);

}


} // namespace pcpe
