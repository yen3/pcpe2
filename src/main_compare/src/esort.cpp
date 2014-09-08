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

/*****************************************************************************/
//  Functions 
/*****************************************************************************/
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


void esort_sort_file_task(const FilenameList& esfl) {
    std::size_t local_esfl_index;
    while(1){
        local_esfl_index = esfl_index.fetch_add(1);
        if(local_esfl_index > esfl.size()){
            break;
        }

        // read file to li_list
        std::vector<LocationInfo> li_list;
        const Filename& fn = esfl[local_esfl_index];
        std::ifstream infile(fn, std::ifstream::in);
        std::copy(std::istream_iterator<LocationInfo>(infile),
                  std::istream_iterator<LocationInfo>(),
                  std::back_inserter(li_list));

        infile.close();
    
        // quick sort for li_list
        std::sort(li_list.begin(), li_list.end());

        // wirte li_list to file and close
        std::ofstream out_file(fn, std::ofstream::out);
        std::copy(li_list.begin(),
                  li_list.end(),
                  std::ostream_iterator<LocationInfo>(out_file, "\n"));
        out_file.close();
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
    const std::size_t WRITE_MAX_SIZE = 100000;
    std::size_t write_count = 0;

    // create LocationInfoFile list
    std::vector<LocationInfoFile> lif_list;
    for(auto fn: fn_list){
        lif_list.push_back(std::move(LocationInfoFile(fn)));
    } 

    // heapify the LocationInfoFile list  first
    std::make_heap(lif_list.begin(), lif_list.end());

    // open the merge file and create empty location infor list
    std::ofstream esort_out(esort_fn, std::ofstream::out);
    std::vector<LocationInfoPtr> li_list;

    while(!lif_list.empty()){
        // pop out the min element from the LocationInfoFile list
        std::pop_heap(lif_list.begin(), lif_list.end());

        LocationInfoFile& lif = lif_list[lif_list.size()-1];
        if(lif.size()){
            LocationInfoPtr min_li= lif.getEntry();
            if(min_li != nullptr){
                li_list.push_back(min_li);
            }
            else{
                std::cerr << "Error in find the min element" << std::endl;
                std::exit(0);
            }
        }
        
        // if the min element entry is empty, remove the LocationInfoFile element 
        if(lif.empty()){
            lif_list.pop_back();
        }

        // pop in the LocationInfoFile entry into the list
        std::push_heap(lif_list.begin(), lif_list.end());

        // the merge array size is WRITE_MAX_SIZE, write to the merge file
        // (Optional) print sepearate operator, if the x and y value of two
        // continued element is not the same.
        if(li_list.size() >= WRITE_MAX_SIZE){
            write_count += li_list.size();
            std::cout << "Write " << write_count << std::endl;

            for(auto& li: li_list){
                esort_out << *li << "\n";
            }
            li_list.clear();
        }
    }

    // if the LocationInfoFile list is not empty, write the remaining element
    // to the merge file
    if(li_list.size() > 0){
        for(auto& li: li_list){
            esort_out << *li << "\n";
        }
        li_list.clear();
    }

    // and close the merge file
    esort_out.close();
}

void esort(std::shared_ptr<FilenameList> fn_list,
           const Filename esort_fn)
{
    // sort each file which contain partail subcommon subseqencs parallely.
    std::cout << "esrot parallel - start" << std::endl;
    //esort_sort_file(*fn_list);
    std::cout << "esrot parallel - end" << std::endl;

    // merge these sorted file into a file.
    std::cout << "esrot merge files - start" << std::endl;
    esort_merge_sort_files(*fn_list, esort_fn);
    std::cout << "esrot merge files - end" << std::endl;
}

}

#if defined(__GTEST_PCPE__)
#endif  /* __GTEST_PCPE__  */
