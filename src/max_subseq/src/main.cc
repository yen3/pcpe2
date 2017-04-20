#include <iostream>
#include <vector>

#include "logging.h"
#include "small_seq_hash.h"
#include "pcpe_util.h"

int main(int argc, char *argv[]) {
  // Init the logging environment.
  pcpe::InitLogging(pcpe::LoggingLevel::kDebug);

  std::vector<pcpe::FilePath> cs_filepaths;
  pcpe::FilePath xfilepath("../../example/influenza_seq_css.txt");
  pcpe::FilePath yfilepath("../../example/protein_seq_css.txt");

  pcpe::comsubseq_smallseqs(xfilepath, yfilepath, cs_filepaths);

  return 0;
}
