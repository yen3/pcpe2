#include <iostream>
#include <fstream>
#include <iterator>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <array>
#include <algorithm>

#include "pcpe_util.h"
#include "esort.h"

namespace pcpe {

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
std::atomic_uint esfl_index(0);

/*****************************************************************************/
//  Functions
/*****************************************************************************/
void esort_sort_file(const Filename& fn) {
    static const std::size_t kBufferSize = 100000;

    // read file to li_list
    std::vector<ComSubseq> li_list;
    ComSubseqFileReader::readFile(fn, li_list, kBufferSize);

    // quick sort for li_list
    std::sort(li_list.begin(), li_list.end());

    // write file to li_list
    ComSubseqFileWriter::writeFile(fn, li_list, kBufferSize);
}

void esort_sort_file_task(const FilenameList& esfl) {
    std::size_t local_esfl_index;
    while (1) {
        local_esfl_index = esfl_index.fetch_add(1);
        if (local_esfl_index >= esfl.size()) {
            break;
        }
#if defined(__DEBUG__) && !defined(__GTEST_PCPE__)
        std::cout << local_esfl_index << std::endl;
#endif

        esort_sort_file(esfl[local_esfl_index]);
    }
}

void esort_sort_files(const FilenameList& fn_list) {
    std::vector<std::thread> tasks(std::thread::hardware_concurrency());
    for (auto& task : tasks) {
        task = std::thread(esort_sort_file_task, fn_list);
    }
    for (auto& task : tasks) {
        task.join();
    }
}

void esort_merge_sort_files(const FilenameList& fn_list,
                            const Filename& esort_fn,
                            const std::size_t kBufferSize = 100000) {
#if defined(__DEBUG__) && !defined(__GTEST_PCPE__)
    std::size_t write_count = 0;
#endif

    // create read_file list
    std::vector<ComSubseqFileReader> csfr_list;
    for (auto fn : fn_list) {
        csfr_list.push_back(ComSubseqFileReader(fn, kBufferSize));
    }

    // heapify the read_file list first
    std::make_heap(csfr_list.begin(), csfr_list.end(),
                   ComSubseqFileReader::esortMergeCompare);

    // open the merge output file and create empty location infor list
    ComSubseqFileWriter esort_out(esort_fn, kBufferSize);
    while (!csfr_list.empty()) {
        // get the min element from these files
        ComSubseq seq;
        std::pop_heap(csfr_list.begin(), csfr_list.end(),
                      ComSubseqFileReader::esortMergeCompare);
        csfr_list.back().readSeq(seq);

        // write the min element to the merge output file
        esort_out.writeSeq(seq);

#if defined(__DEBUG__) && !defined(__GTEST_PCPE__)
        if (write_count++ % 100000 == 0) {
            std::cout << __FILE__ << " " << __LINE__ << ": "
                      << "write " << write_count << " " << csfr_list.size()
                      << std::endl;
        }
#endif

        // if the reading state of the file is eof, the list would remove the
        // file. Otherwise it push heap the file
        if (csfr_list.back().eof()) {
            csfr_list.pop_back();
        } else {
            std::push_heap(csfr_list.begin(), csfr_list.end(),
                           ComSubseqFileReader::esortMergeCompare);
        }
    }

    // clean the write buffer and close the file
    esort_out.close();
}

void esort(std::shared_ptr<FilenameList> fn_list, const Filename esort_fn) {
    // sort each file which contain partail subcommon subseqencs parallely.
    std::cout << "esrot parallel - start" << std::endl;
    esort_sort_files(*fn_list);
    std::cout << "esrot parallel - end" << std::endl;

    // merge these sorted file into a file.
    std::cout << "esrot merge files - start" << std::endl;
    esort_merge_sort_files(*fn_list, esort_fn);
    std::cout << "esrot merge files - end" << std::endl;
}
}
