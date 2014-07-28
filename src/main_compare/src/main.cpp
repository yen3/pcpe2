#include <iostream>
#include <string>
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

    auto commom_subseq_fn_list = pcpe::common_subseq(input_fn_a, input_fn_b);

    pcpe::Filename esort_result;
    pcpe::esort(commom_subseq_fn_list, esort_result);

#if 0
    pcpe::Filename common_subseq_result = pcpe::maximum_common_subseq(esort_result);
#endif


    return 0;
}

