#include <iostream>
#include <vector>

#include "logging.h"
#include "small_seq_hash.h"
#include "com_subseq_sort.h"
#include "max_comsubseq.h"
#include "env.h"
#include "pcpe_util.h"

void InitEnvironment(int argc, char* argv[]) {
  // Init the logging environment.
  pcpe::InitLogging(pcpe::LoggingLevel::kDebug);

  // Create temp folder
  const pcpe::FilePath& temp_folder = pcpe::gEnv.getTempFolderPath();
  if (!pcpe::CheckFolderExists(temp_folder.c_str()))
    pcpe::CreateFolder(temp_folder.c_str());

  if (argc <= 3) {
    LOG_ERROR() << "Need two input file and one output filepath." << std::endl;
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  InitEnvironment(argc, argv);

  pcpe::FilePath xfilepath(argv[1]);
  pcpe::FilePath yfilepath(argv[2]);
  pcpe::FilePath ofilepath(argv[3]);

  std::vector<pcpe::FilePath> cs_filepaths;
  pcpe::CompareSmallSeqs(xfilepath, yfilepath, cs_filepaths);

  std::vector<pcpe::FilePath> cs_sorted_filepaths;
  pcpe::SortComSubseqsFiles(cs_filepaths, cs_sorted_filepaths);

  std::vector<pcpe::FilePath> max_comsubseq_filepaths;
  pcpe::MaxSortedComSubseqs(cs_sorted_filepaths, max_comsubseq_filepaths);

  pcpe::CombineComSubSeqFiles(max_comsubseq_filepaths, ofilepath);

  return 0;
}
