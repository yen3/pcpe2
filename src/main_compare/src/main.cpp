#include <iostream>
#include <string>
#include "hash_table.h"

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


    return 0;
}

