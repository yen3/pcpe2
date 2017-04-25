#include <iostream>
#include <vector>

#include "logging.h"
#include "small_seq_hash.h"
#include "pcpe_util.h"

int main(int argc, char *argv[]) {
  // Init the logging environment.
  pcpe::InitLogging(pcpe::LoggingLevel::kDebug);

  if (argc <= 3) {
    LOG_ERROR() << "Need two input file." << std::endl;
    return 1;
  }

  std::vector<pcpe::FilePath> cs_filepaths;
  pcpe::FilePath xfilepath(argv[1]);
  pcpe::FilePath yfilepath(argv[2]);

  pcpe::CompareSmallSeqs(xfilepath, yfilepath, cs_filepaths);

  return 0;
}
