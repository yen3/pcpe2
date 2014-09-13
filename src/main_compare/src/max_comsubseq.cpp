#include <vector>
#include <string>
#include <array>

#include "pcpe_util.h"
#include "max_comsubseq.h"

namespace pcpe {

#if 0
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
#endif

void maximum_common_subseq(const Filename& esort_result,
                           const Filename& reduce_result)
{
    const std::size_t kReadMaxSize = 1000000;
    const std::size_t kWriteBufferSize = 1000000;

    // open the read file
    std::ifstream infile(esort_result, std::ifstream::in | std::ifstream::binary);

    // open the reduce output file
    ComSubseqFileWriter outfile(reduce_result, kWriteBufferSize);

    std::size_t remaining_size = 0;
    std::array<ComSubseq, kReadMaxSize> com_list;
    std::array<bool, kReadMaxSize> reduced_list;
    std::size_t com_list_size = 0;
    while(!infile.eof()){
        reduced_list.fill(false);

        // read the file with READ_MAX_SIZE - remaining size 
        infile.read(static_cast<char*>(static_cast<void*>(&com_list[remaining_size])),
                    sizeof(ComSubseq) * (kReadMaxSize - remaining_size));
        std::size_t read_size = infile.gcount() / sizeof(ComSubseq);
        com_list_size = remaining_size + read_size;

        // find the lastest index of the part (from READ_MAX_SIZE to 0)
        // if x1 != x2 and y1 != y2
        std::size_t handle_size = com_list_size;
        for(; com_list[handle_size-1].isContinued(com_list[handle_size-2]); --handle_size) ;
        remaining_size = com_list_size - handle_size;

        // start to reduce
        for(std::size_t i=0; i<handle_size; ++i){
            if(!reduced_list[i]){
                std::size_t l = com_list[i].getLength();
                for(std::size_t j=i; j+1 < handle_size && com_list[j].isContinued(com_list[j+1]); j++){
                    reduced_list[j+1] = true;
                    l++; 
                }

                com_list[i].setLength(l);
            }
        }

        // write the reduce part to file
        for(std::size_t i=0; i<handle_size; ++i){
            if(!reduced_list[i]){
                outfile.writeSeq(com_list[i]);
            }
        }

        // move the remaining to the begin of the vector
        for(std::size_t i=0, r=handle_size;
            r< handle_size+remaining_size;
            ++i, ++r){
            com_list[i] = com_list[r];
        }
    }
    infile.close();
    outfile.close();
}

} // namespace pcpe

