#ifndef __ESORT_H__
#define __ESORT_H__

#include <vector>
#include <string>
#include "pcpe_util.h"

namespace pcpe{

void esort(std::shared_ptr<FilenameList> fn_list,
           const Filename esort_fn);

}

#endif
