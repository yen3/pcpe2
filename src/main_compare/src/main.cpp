#include <iostream>
#include <string>
#include <sstream>
#include "hash_table.h"
#include "esort.h"
#include "max_comsubseq.h"


namespace pcpe
{

void common_peptide_explorer(const Filename& input_seq_x_fn,
                             const Filename& input_seq_y_fn,
                             const Filename& output_fn) {
    Filename subseq_prefix;
    Filename esort_result_fn;

    get_esort_output_file_name(input_seq_x_fn, input_seq_y_fn,
                               esort_result_fn);

    std::cout << "Create common subseq files - start" << std::endl;
    FilenameList common_subseq_fn_list;
    common_subseq(input_seq_x_fn, input_seq_y_fn, common_subseq_fn_list);
    std::cout << "Create common subseq files - end" << std::endl;

    std::cout << "esort subseqeunce files - start" << std::endl;
    //Filename esort_result = "esort_test.txt";
    esort(common_subseq_fn_list, esort_result_fn);
    std::cout << "esort subseqeunce files - end" << std::endl;

    std::cout << "find all maximum subseqeunce - start" << std::endl;
    //Filename common_subseq_result = "common_subseq_result.txt";
    maximum_common_subseq(esort_result_fn, output_fn);
    std::cout << "find all maximum subseqeunce - end" << std::endl;
}

} // namespace pcpe


#ifndef __GTEST_PCPE__
int main(int argc, char const* argv[])
{

    /**
     *   argv[1]: input sequence a
     *   argv[2]: input sequence b
     *   argv[3]: output file
     * */
    pcpe::Filename input_fn_a;
    pcpe::Filename input_fn_b;
    pcpe::Filename output_fn;

    if(argc == 4){
        input_fn_a = argv[1];
        input_fn_b = argv[2];
        output_fn = argv[3];

        std::cout << "./pcpe2 input_sequence_filename_x input_sequence_filename_y output_filename" << std::endl;
    }
    else{
        return -1;
    }

    pcpe::common_peptide_explorer(input_fn_a, input_fn_b, output_fn);

    return 0;
}
#endif
