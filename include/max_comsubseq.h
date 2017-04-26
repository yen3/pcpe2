#pragma once

#include <vector>

#include "pcpe_util.h"

namespace pcpe {

/**
 * Find the maximum common subseqences for each file.
 *
 * If the seqences of ComSubseq are sorted, the function can merge the
 * contineous ComSubseqs.
 *
 * For example, there are two sequences "ABCDEFGHI" and "BCDEFGHI".
 * The two of these ComSubseqs are
 *
 *      x    y  x_loc  y_loc  len
 *     (2,   1,     2,     0,   6) "BCDEFGH"
 *     (2,   1,     3,     1,   6) "CDEFGHI"
 *
 * The two ComSubseqs are continous so we can merge the two ComSubseqs into a
 * one ComSubseq.
 *
 *      x    y  x_loc  y_loc  len
 *     (2,   1,     2,    0,    7) "BCDEFGHI"
 *
 * @param[in] ifilepaths The list of input filepaths. The sequences of each
 *                       file must be sorted.
 * @param[out] ofilepaths The list of output filepaths.
 *
 * */
void MaxSortedComSubseqs(
    const std::vector<FilePath>& ifilepaths,
    std::vector<FilePath>& ofilepaths);

} // namespace pcpe
