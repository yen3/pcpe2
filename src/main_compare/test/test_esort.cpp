#if defined(__GTEST_PCPE__)
#include "hash_table.h"
#include "pcpe_util.h"
#include "esort.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <memory>

#include <gtest/gtest.h>

namespace pcpe{

extern void esort_sort_file_task(const FilenameList& esfl);
extern void esort_sort_files(const FilenameList& fn_list);
extern void esort_merge_sort_files(const FilenameList& fn_list,
                                   const Filename& esort_fn);
extern void esort(std::shared_ptr<FilenameList> fn_list,
                  const Filename esort_fn);

TEST(esort, esort_init){

}

}

#endif /* __GTEST_PCPE__ */
