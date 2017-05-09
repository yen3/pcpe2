#pragma once

#include <vector>
#include "pcpe_util.h"

namespace pcpe {
/**
 * Sort ComSubseqs for each file.
 *
 * If the sequence of subseqences are sorted, the program can find the maximum
 * common subseqence by checking the continuous seqence.
 *
 * @param[in] input_filepaths The list of input filepaths.
 * @param[out] sorted_filepaths The list of output filepaths. The sequences
 *                              of each file is sorted.
 *
 * */
void SortComSubseqsFiles(const std::vector<FilePath>& input_filepaths,
                         std::vector<FilePath>& sorted_filepaths);

} // namespace pcpe
