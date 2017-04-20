#pragma once

#include <cstdint>
#include <string>

namespace pcpe {

typedef std::string Filepath;

/**
 * Chech the file of the path exists or not
 *
 * @param filepath the path of the file
 *
 * @return true: exists
 *         false: not exists
 * */
bool ChechFileExists(const char* filepath);

/**
 * Get file size from a file path.
 *
 * @param filepath the path of the file
 *
 * @return -1: present error
 *         otherwise: the size (unit: byte(s)) of the file
 */
int64_t GetFileSize(const char* filepath);

} // namespace pcpe

