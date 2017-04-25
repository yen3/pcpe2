#include "pcpe_util.h"

#include <fstream>

#include <sys/stat.h>
#include <unistd.h>

namespace pcpe {

bool ChechFileExists(const char* path) {
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

bool GetFileSize(const char* path, FileSize& size) {
  if (!ChechFileExists(path))
    return false;

  std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
  size = in.tellg();
  in.close();

  return true;
}

} // namepspace pcpe
