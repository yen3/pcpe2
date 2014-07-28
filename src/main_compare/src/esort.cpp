#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <atomic>
#include <array>

#include "esort.h"

namespace pcpe{

/*****************************************************************************/
// Global Variables
/*****************************************************************************/
std::atomic_uint esfl_index(0);

/*****************************************************************************/
//  Classes
/*****************************************************************************/
class LocationInfo{
public:
    bool operator<(const LocationInfo& rhs) const
    {
        int c[4] = {x_ - rhs.x_,
                    y_ - rhs.y_,
                    x_loc_ - rhs.x_loc_,
                    y_loc_ - rhs.y_loc_};

        for(std::size_t cidx= 0; cidx< 4; ++cidx){
            if(c[cidx]  != 0){
                return c[cidx] < 0;
            } 
        }
        return false;
    }

    bool operator>(const LocationInfo& rhs) const
    {
        int c[4] = {x_ - rhs.x_,
                    y_ - rhs.y_,
                    x_loc_ - rhs.x_loc_,
                    y_loc_ - rhs.y_loc_};

        for(std::size_t cidx= 0; cidx< 4; ++cidx){
            if(c[cidx]  != 0){
                return c[cidx] > 0;
            } 
        }
        return false;
    }

    bool operator==(const LocationInfo& rhs) const
    {
        return (x_ == rhs.x_) && (y_ == rhs.y_) &&
               (x_loc_ == rhs.x_loc_) && (y_loc_ && rhs.y_loc_);
    }
    
    bool operator>=(const LocationInfo& rhs) const { return !(*this < rhs); }
    bool operator<=(const LocationInfo& rhs) const { return !(*this >= rhs); }
    bool operator!=(const LocationInfo& rhs) const { return !(*this == rhs); }

private:
    int x_;
    int y_;
    int x_loc_;
    int y_loc_;
}; 

class LocationInfoFile{
    static const std::size_t READ_MAX_ENTRY_SIZE = 10000;
public:

private:
    std::array<LocationInfo, READ_MAX_ENTRY_SIZE> li_;
    std::size_t li_size_;
    std::ifstream infile_;
};

/*****************************************************************************/
//  Functions 
/*****************************************************************************/
void esort_sort_file_task(const FilenameList& esfl)
{
    std::size_t local_esfl_index;
    while(1){
        local_esfl_index = esfl_index.fetch_add(1);
        if(local_esfl_index > esfl.size()){
            break;
        }

        // read file

        // quick sort

        // wirte file and close
    
    }
}

void esort_sort_file(const FilenameList& fn_list)
{
    std::vector<std::thread> tasks(std::thread::hardware_concurrency());
    for(auto& task : tasks){
        task = std::thread(esort_sort_file_task, fn_list);
    }
    for(auto& task : tasks){
        task.join();
    }
}

void esort_merge_sort_files(const FilenameList& fn_list,
                            const Filename& esort_fn)
{

}

void esort(std::shared_ptr<FilenameList> fn_list,
           const Filename esort_fn)
{
    esort_sort_file(*fn_list);

    esort_merge_sort_files(*fn_list, esort_fn);
}

}
