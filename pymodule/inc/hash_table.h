#ifndef __HASH_TABLE_H__
#define __HASH_TABLE_H__

#include <vector>
#include <string>
#include <map>
#include <utility>
#include "pcpe_util.h"

namespace pcpe{

const std::size_t TASK_SIZE = 100;
const std::size_t HASH_TABLE_SIZE= 100003;
const std::size_t SUBSTRING_SIZE = 6;

/*****************************************************************************/
// Typedef  
/*****************************************************************************/
class CommonSubseqTask;

typedef std::string Seq;
typedef std::vector<Seq> SeqList;

typedef std::size_t SeqIndex;
typedef std::size_t SubstrIndex;

/* 
 * key:    hash value
 * value:  <substring (6 chars), <sequence index, substring location index> >
 * */
typedef std::vector<
    std::map<Seq, std::vector<std::pair<SeqIndex, SubstrIndex> > > > HashTable;

class CommonSubseqTask;
typedef std::vector<CommonSubseqTask> CommonSubseqTaskList;

class CommonSubseqTask{
    friend std::ostream& operator<<(std::ostream& os,
                                    const CommonSubseqTask& cst);
public:
    CommonSubseqTask(std::size_t begin_index,
                     std::size_t end_index,
                     const Filename& filename):begin(begin_index),
                                               end(end_index),
                                               fn(filename)
    {
    };

    const std::size_t begin;
    const std::size_t end;
    const Filename fn;
};


/**
  *  List all common subsequences whose length are 6.
  *
  *  @param[in]  fn_seq_a   filename of sequence a
  *  @param[in]  fn_seq_b   filename of sequence b
  *
  *  @return  a share pointer to a filename list. Each file coantains part of
  *           common subsequences, but not sorted. If you want to use these
  *           informatinos, you have to sort these files and merge sort them to
  *           locate the maximum subsequences.
  * 
  */
std::shared_ptr<std::vector<Filename> > common_subseq(const Filename& fn_seq_a,
                                                      const Filename& fn_seq_b,
                                                      const Filename& temp_file_prefix="subhash");

}

#endif
