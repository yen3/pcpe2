#include "pcpe_util.h"

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

namespace pcpe {

bool ChechFileExists(const char* path) {
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

int64_t GetFileSize(const char* path) {
  if (!ChechFileExists(path))
    return -1;

  std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
  int64_t fsize = in.tellg();
  in.close();

  return fsize;
}

} // namepspace pcpe
