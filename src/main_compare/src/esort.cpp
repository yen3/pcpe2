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

namespace pcpe{

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
std::atomic_uint esfl_index(0);

#if 0
/*****************************************************************************/
//  Classes
/*****************************************************************************/
class LocationInfoFile{
    static const std::size_t READ_MAX_SIZE = 100000;

public:
    LocationInfoFile(const Filename& fn);
    LocationInfoPtr getEntry();
    bool empty();
    inline std::size_t size() const { return li_size_; };

    bool operator<(const LocationInfoFile& rhs)  const { return *li_[0] < *rhs.li_[0];  }
    bool operator>(const LocationInfoFile& rhs)  const { return *li_[0] > *rhs.li_[0];  }
    bool operator==(const LocationInfoFile& rhs) const { return *li_[0] == *rhs.li_[0]; }
    bool operator>=(const LocationInfoFile& rhs) const { return !(*this < rhs);         }
    bool operator<=(const LocationInfoFile& rhs) const { return !(*this >= rhs);        }
    bool operator!=(const LocationInfoFile& rhs) const { return !(*this == rhs);        }

private:
    std::array<LocationInfoPtr, READ_MAX_SIZE> li_;
    std::size_t li_size_;
    std::ifstream infile_;
};
#endif

/*****************************************************************************/
//  Functions 
/*****************************************************************************/
#if 0
LocationInfoFile::LocationInfoFile(const Filename& fn):
    li_size_(0),
    infile_(fn, std::ifstream::in) {

    // read the array with maximum size  
    LocationInfoPtr lip;
    for(li_size_ = 0; !infile_.eof() && li_size_ < READ_MAX_SIZE; ++li_size_){
        li_[li_size_] = LocationInfoPtr(new LocationInfo());
        infile_ >> *li_[li_size_];
    }
    std::reverse(li_.begin(), li_.begin()+li_size_);
}


bool LocationInfoFile::empty() {
    return !infile_.is_open() && li_size_ == 0;
}


LocationInfoPtr LocationInfoFile::getEntry() {
    if(empty()){
        std::cerr << "Error in query the min element" << std::endl;
        return nullptr;
    }

    // pop the min entry and size - 1
    LocationInfoPtr min_loc = li_[li_size_-1];
    li_[li_size_-1] = nullptr;
    li_size_--;
    
    // if the array size is 0, read the file with max read size 
    if(li_size_ == 0 && infile_.is_open()){
        for(li_size_ = 0; !infile_.eof() && li_size_ < READ_MAX_SIZE; ++li_size_){
            li_[li_size_] = LocationInfoPtr(new LocationInfo());
            infile_ >> *li_[li_size_];
        }
        std::reverse(li_.begin(), li_.begin()+li_size_);
        
        if(infile_.eof()){
            infile_.close();
        }
    }

    return min_loc;
}
#endif


void esort_sort_file_task(const FilenameList& esfl) {
    std::size_t local_esfl_index;
    while(1){
        local_esfl_index = esfl_index.fetch_add(1);
        if(local_esfl_index >= esfl.size()){
            break;
        }

        // read file to li_list
        std::vector<ComSubseq> li_list;
        const Filename& fn = esfl[local_esfl_index];
        ComSubseqFile::readFile(fn, li_list);

        // quick sort for li_list
        std::sort(li_list.begin(), li_list.end());

        // write file to li_list
        ComSubseqFile::writeFile(fn, li_list);
    }
}

void esort_sort_file(const FilenameList& fn_list) {
    std::vector<std::thread> tasks(std::thread::hardware_concurrency());
    for(auto& task : tasks){
        task = std::thread(esort_sort_file_task, fn_list);
    }
    for(auto& task : tasks){
        task.join();
    }
}

void esort_merge_sort_files(const FilenameList& fn_list,
                            const Filename& esort_fn) {
    std::size_t write_count = 0;

    // create read_file list
    std::vector<ComSubseqFileReader> csfr_list;
    for(auto fn: fn_list){
        csfr_list.push_back(std::move(ComSubseqFileReader(fn)));
    }
    
    // heapify the read_file list first
    std::make_heap(csfr_list.begin(), csfr_list.end());

    // open the merge output file and create empty location infor list
    ComSubseqFileWriter esort_out(esort_fn);
    while(!csfr_list.empty()){
        // get the min element from these files
        ComSubseq seq;
        std::pop_heap(csfr_list.begin(), csfr_list.end());
        csfr_list[csfr_list.size()-1].readSeq(seq);

        // if the reading state of the file is eof, the list would remove the
        // file. Otherwise it push heap the file 
        if(csfr_list[csfr_list.size()-1].eof()){
            csfr_list.pop_back();
        }
        lse{
            std::push_heap(csfr_list.begin(), csfr_list.end());
        }

        // write the min element to the merge output file
        esort_out.writeSeq(seq);
        if(write_count++ % 10000 == 0){
            std::cout << "write " << write_count << std::endl;
        }
    }

    // clean the write buffer and close the file
    esort_out.close();
}

void esort(std::shared_ptr<FilenameList> fn_list,
           const Filename esort_fn)
{
    // sort each file which contain partail subcommon subseqencs parallely.
    std::cout << "esrot parallel - start" << std::endl;
    esort_sort_file(*fn_list);
    std::cout << "esrot parallel - end" << std::endl;

    // merge these sorted file into a file.
    std::cout << "esrot merge files - start" << std::endl;
    esort_merge_sort_files(*fn_list, esort_fn);
    std::cout << "esrot merge files - end" << std::endl;
}

}

#if defined(__GTEST_PCPE__)
#endif  /* __GTEST_PCPE__  */

