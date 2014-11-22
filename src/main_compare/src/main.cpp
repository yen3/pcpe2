#include <iostream>
#include <string>
#include <sstream>
#include "hash_table.h"
#include "esort.h"
#include "max_comsubseq.h"

int main(int argc, char const* argv[])
{

    /**
     *   argv[1]: input sequence a
     *   argv[2]: input sequence b
     *   argv[3]: output file
     * */
    std::string input_fn_a;
    std::string input_fn_b;
    std::string output_fn;

    if(argc == 4){
        input_fn_a = argv[1];
        input_fn_b = argv[2];
        output_fn = argv[3];
    }
    else{
        return -1;
    }

    std::cout << "Create common subseq files - start" << std::endl;
    auto commom_subseq_fn_list = pcpe::common_subseq(input_fn_a, input_fn_b);
    std::cout << "Create common subseq files - end" << std::endl;

    std::cout << "esort subseqeunce files - start" << std::endl;
    pcpe::Filename esort_result = "esort_test.txt";
    pcpe::esort(commom_subseq_fn_list, esort_result);
    std::cout << "esort subseqeunce files - end" << std::endl;

#if 0
    std::cout << "find all maximum subseqeunce - start" << std::endl;
    pcpe::Filename common_subseq_result = "common_subseq_result.txt";
    pcpe::maximum_common_subseq(esort_result, common_subseq_result);
    std::cout << "find all maximum subseqeunce - end" << std::endl;
#endif

    return 0;
}

