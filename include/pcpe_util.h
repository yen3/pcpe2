#pragma once

#include <string>
#include <fstream>

namespace pcpe {

typedef std::string FilePath;
typedef std::ifstream::pos_type FileSize;

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
 * Chech the folder of the path exists or not
 *
 * @param filepath the path of the folder
 *
 * @return true: exists
 *         false: not exists
 * */
bool ChechFolderExists(const char* filepath);

/**
 * Get file size from a file path.
 *
 * @param[in] filepath the path of the file
 * @param[out] size the size of the file (unit: byte(s))
 *
 * @return false: present error
 *         ture: the size (unit: byte(s)) of the file
 */
bool GetFileSize(const char* filepath, FileSize& size);

/**
 * Create the folder of the path recursively.
 *
 * @param filepath the path of the folder
 *
 * @return false: present error
 *         ture: the size (unit: byte(s)) of the file
 */
bool CreateFolder(const char* path);

} // namespace pcpe

