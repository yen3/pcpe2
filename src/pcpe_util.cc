#include "pcpe_util.h"

#include <cstring>
#include <fstream>
#include <memory>

#include <sys/stat.h>
#include <unistd.h>

namespace pcpe {

bool CheckFileExists(const char* path) {
  struct stat buffer;
  return (stat(path, &buffer) == 0);
}

bool CheckFolderExists(const char* filepath) {
  struct stat file_stat;

  if (stat(filepath, &file_stat) != 0)
    return false;

  if (S_ISDIR(file_stat.st_mode))
    return true;
  else
    return false;
}

bool CreateFolder(const char* path) {
  // Copy the path
  std::size_t tmp_size = std::strlen(path);
  std::unique_ptr<char[]> tmp(new char[tmp_size + 1]);
  std::memcpy(tmp.get(), path, sizeof(char) * (tmp_size + 1));

  if (tmp[tmp_size -1] == '/') {
    tmp[tmp_size - 1] = 0;
    tmp_size--;
  }

  for (char* p = tmp.get(); *p != 0; ++p) {
		if (*p == '/') {
			*p = 0;
			mkdir(tmp.get(), S_IRWXU);
			*p = '/';
    }
  }

	if (!CheckFolderExists(tmp.get())) {
		int mk_status = mkdir(tmp.get(), S_IRWXU);
		return mk_status == 0;
  }
  return false;
}

bool GetFileSize(const char* path, FileSize& size) {
  if (!CheckFileExists(path))
    return false;

  std::ifstream in(path, std::ifstream::ate | std::ifstream::binary);
  size = in.tellg();
  in.close();

  return true;
}

bool CheckFileNotEmpty(const char* path) {
  FileSize file_size;
  if (!GetFileSize(path, file_size))
    return false;

  if (file_size == 0)
    return false;

  return true;
}

} // namepspace pcpe
