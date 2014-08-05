#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include "pcpe_util.h"

namespace pcpe{


std::istream& operator>>(std::istream& in, LocationInfo& li)
{
    in >> li.x_ >> li.y_ >> li.x_loc_ >> li.y_loc_;
    return in;
}


std::ostream& operator<<(std::ostream& out, const LocationInfo& li) 
{
    out << li.x_ << " " << li.y_ << " " << li.x_loc_ << " " << li.y_loc_;
    return out; 
}


} // namespace pcpe

