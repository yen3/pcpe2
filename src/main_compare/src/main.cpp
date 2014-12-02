#include <iostream>
#include <string>
#include <sstream>
#include "hash_table.h"
#include "esort.h"
#include "max_comsubseq.h"


#include <algorithm>
#include <sstream>
#include <unistd.h>
#include <libgen.h>

namespace pcpe {

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
        std::cout << "Get PWD error !" << std::endl;
    }

    current_path = cwd;
}


void get_file_basename(const Filename& path, Filename& output_basename) {
    char* base_name = new char[path.size() + 1];
    std::fill(base_name, base_name + path.size() + 1, 0);
    std::copy(path.begin(), path.end(), base_name);

    base_name = basename(base_name);
    output_basename = base_name;

    delete [] base_name;
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
    std::cout << cwd << std::endl;

    Filename pid_prefix;
    get_pid_filename_prefix(pid_prefix);
    std::cout << pid_prefix << std::endl;

    Filename basename_x;
    get_file_basename_without_sufix(input_seq_x_fn, basename_x);
    std::cout << basename_x << std::endl;

    Filename basename_y;
    get_file_basename_without_sufix(input_seq_y_fn, basename_y);
    std::cout << basename_y << std::endl;

    std::ostringstream os;
    os << cwd << "_" << pid_prefix << "_" << basename_x << "_" << basename_y
       << "_hash_"; 

    output_prefix = os.str();

    std::cout << output_prefix << std::endl;
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

}

void common_peptide_explorer(const Filename& input_seq_x_fn,
                             const Filename& input_seq_y_fn,
                             const Filename& output_fn) {

    std::cout << "Create common subseq files - start" << std::endl;
    auto commom_subseq_fn_list =
        pcpe::common_subseq(input_seq_x_fn, input_seq_y_fn);
    std::cout << "Create common subseq files - end" << std::endl;

    std::cout << "esort subseqeunce files - start" << std::endl;
    pcpe::Filename esort_result = "esort_test.txt";
    pcpe::esort(commom_subseq_fn_list, esort_result);
    std::cout << "esort subseqeunce files - end" << std::endl;

    std::cout << "find all maximum subseqeunce - start" << std::endl;
    pcpe::Filename common_subseq_result = "common_subseq_result.txt";
    pcpe::maximum_common_subseq(esort_result, common_subseq_result);
    std::cout << "find all maximum subseqeunce - end" << std::endl;

}

} // namespace pcpe


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
    }
    else{
        return -1;
    }

    pcpe::Filename output;
    pcpe::get_common_subseq_output_prefix(input_fn_a, input_fn_b, output);
    std::cout << output << std::endl;
    //pcpe::common_peptide_explorer(input_fn_a, input_fn_b, output_fn);

    return 0;
}

