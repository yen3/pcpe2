#include <vector>
#include <string>
#include <array>

#include "pcpe_util.h"
#include "max_comsubseq.h"

namespace pcpe {

class LocationInfoLength;

std::istream& operator>>(std::istream& in, LocationInfoLength& li);
std::ostream& operator<<(std::ostream& out, const LocationInfoLength& li);

class LocationInfoLength : public LocationInfo
{
    friend std::istream& operator>>(std::istream& in, LocationInfoLength& li);
    friend std::ostream& operator<<(std::ostream& out, const LocationInfoLength& li);
public:
    LocationInfoLength():LocationInfo(),
                         length_(kInitialCommonLength),
                         reduced_(false){};
    bool is_continue(const LocationInfoLength& li){
        return (x_ == li.x_) && (y_ == li.y_) &&
               (li.x_loc_ - x_loc_ == 1)  && (li.y_loc_ - y_loc_ == 1);
    };
    inline bool is_same_x_y(const LocationInfoLength& li){
        return x_ == li.x_ && y_ == li.y_;
    };
    inline void set_reduce(){ reduced_ = true; };
    inline bool is_reduced(){ return reduced_; };
    inline void set_length(std::size_t l){ length_ = l; };
    inline std::size_t get_length(){ return length_; };
private:
    static const int kInitialCommonLength = 6;
    std::size_t length_;
    bool reduced_;
};


std::istream& operator>>(std::istream& in, LocationInfoLength& li)
{
    in >> li.x_ >> li.y_ >> li.x_loc_ >> li.y_loc_;
    return in;
}

std::ostream& operator<<(std::ostream& out, const LocationInfoLength& li)
{
    out << " " << li.x_ << " " << li.y_ << " " << li.x_loc_ << " " << li.y_loc_ << li.length_;
    return out;
}


void maximum_common_subseq(const Filename& esort_result,
                           const Filename& reduce_result)
{
    const std::size_t kReadMaxSize = 1000000;

    // open the read file 
    std::ifstream esort_in(esort_result, std::ifstream::in);
    
    // open the reduce output file 
    std::ofstream reduce_out(reduce_result, std::ofstream::out);

    std::size_t remaining_size = 0;
    std::array<LocationInfoLength, kReadMaxSize> li_list;
    std::size_t li_list_size= 0;

    while(1){
        // read the file with READ_MAX_SIZE - remaining size 
        for(li_list_size = remaining_size;
            li_list_size < kReadMaxSize && !esort_in.eof();
            ++li_list_size){
            esort_in >> li_list[li_list_size];
        }

        // find the lastest index of the part (from READ_MAX_SIZE to 0)
        // if x1 != x2 and y1 != y2
        std::size_t handle_size = li_list_size;
        for(; li_list[handle_size-1].is_continue(li_list[handle_size-2]); --handle_size) ;
        remaining_size = li_list_size - handle_size;

        // start to reduce
        for(std::size_t i=0; i<handle_size; ++i){
            if(!li_list[i].is_reduced()){
                std::size_t l = li_list[i].get_length();
                for(std::size_t j=i; j+1 < handle_size && li_list[j].is_continue(li_list[j+1]); j++){
                    li_list[j+1].set_reduce();
                    l++; 
                }

                li_list[i].set_length(l);
            }
        }

        // write the reduce part to file
        for(std::size_t i=0; i<handle_size; ++i){
            if(!li_list[i].is_reduced()){
                reduce_out << li_list[i] << std::endl;
            }
        }

        // move the remaining to the begin of the vector
        for(std::size_t i=0, r=handle_size;
            r< handle_size+remaining_size;
            ++i, ++r){
            li_list[i] = li_list[r];
        }
    }
}

} // namespace pcpe

