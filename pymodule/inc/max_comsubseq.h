#ifndef __MAX_COMSUBSEQ_H__
#define __MAX_COMSUBSEQ_H__

#include "pcpe_util.h"

namespace pcpe {

void maximum_common_subseq(const Filename& esort_result,
                           const Filename& reduce_result,
                           const std::size_t kReadMaxSize = 1000000,
                           const std::size_t kWriteBufferSize = 1000000);

} // namespace pcpe


#endif
