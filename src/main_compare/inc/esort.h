#ifndef __ESORT_H__
#define __ESORT_H__

#include <vector>
#include "pcpe_util.h"

namespace pcpe
{

#if 0
void esort(std::shared_ptr<FilenameList> fn_list,
           const Filename esort_fn);
#endif

void esort(const FilenameList& fn_list,
           const Filename esort_fn);
} // namespace pcpe

#endif /* __ESORT_H__ */
