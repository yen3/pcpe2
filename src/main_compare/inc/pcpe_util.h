#ifndef __PCPE_UTIL_H__
#define __PCPE_UTIL_H__

#include <fstream>
#include <vector>
#include <string>
#include <memory>

namespace pcpe{

typedef std::string Filename;
typedef std::vector<Filename> FilenameList;

class LocationInfo;
typedef std::shared_ptr<LocationInfo> LocationInfoPtr;

std::istream& operator>>(std::istream& in, LocationInfo& li);
std::ostream& operator<<(std::ostream& out, const LocationInfo& li); 

class LocationInfo{
    friend std::istream& operator>>(std::istream& in, LocationInfo& li);
    friend std::ostream& operator<<(std::ostream& in, const LocationInfo& li);
public:
    LocationInfo():x_(0), y_(0), x_loc_(0), y_loc_(0)
    {
    }

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

protected:
    int x_;
    int y_;
    int x_loc_;
    int y_loc_;
}; 

} // namespace pcpe

#endif
