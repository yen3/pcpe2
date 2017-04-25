#pragma once

#include <string>
#include <fstream>

namespace pcpe {

typedef std::string FilePath;
typedef std::ifstream::pos_type FileSize;

/**
 * Check the file of the path exists or not
 *
 * @param filepath the path of the file
 *
 * @return true: exists
 *         false: not exists
 * */
bool CheckFileExists(const char* filepath);

/**
 * Check the folder of the path exists or not
 *
 * @param filepath the path of the folder
 *
 * @return true: exists
 *         false: not exists
 * */
bool CheckFolderExists(const char* filepath);

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


/**
 * Check the file exists and the size of the file is > 0.
 *
 * It's a help function. It just call `CheckFileExists` and `GetFileSize` to
 * check.
 *
 * @param[in] filepath the path of the file
 *
 * @return false: present error
 *         ture: It's a file with the size is > 0.
 *
 * */
bool CheckFileNotEmpty(const char* path);

} // namespace pcpe

