#include <iostream>
#include <vector>

#include "logging.h"
#include "small_seq_hash.h"
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
    LOG_ERROR() << "Need two input file." << std::endl;
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  InitEnvironment(argc, argv);

  std::vector<pcpe::FilePath> cs_filepaths;
  pcpe::FilePath xfilepath(argv[1]);
  pcpe::FilePath yfilepath(argv[2]);

  pcpe::CompareSmallSeqs(xfilepath, yfilepath, cs_filepaths);

  return 0;
}
