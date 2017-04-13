#ifndef __PCPE2_PYMODULE_H__
#define __PCPE2_PYMODULE_H__

#include "pcpe_util.h"

namespace pcpe
{

void common_peptide_explorer(const Filename& input_seq_x_fn,
                             const Filename& input_seq_y_fn,
                             const Filename& output_fn);

}

#endif /* __PCPE2_PYMODULE_H__ */
