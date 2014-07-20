#include <vector>
#include <string>

namespace pcpe{

typedef std::string Filename;

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
std::shared_ptr<std::vector<Filename> > common_subseq(Filename fn_seq_a,
                                                      Filename fn_seq_b);

}

