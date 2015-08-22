#pragma once

#include <string>

//! Stores file information.
struct FileInfo
{
    std::string name;       //!< File name.
    std::string fullName;   //!< Full file name (with directory). Note: may not be absolute.
    bool isDirectory;       //!< True if directory.
};

//! Quick tool to extract file extension.
std::string fileExtension(const std::string& file);

//! Returns true if directory was successfully created.
bool createDirectory(const std::wstring& directory);

//! Returns true if file exists.
bool fileExists(const std::string& filename);
bool fileExists(const std::wstring& filename);

//! List all files and directories.
//! If recursive, be sure that there is no loop with symlink.
std::vector<FileInfo> listFiles(const std::string& directory, bool recursive = false);

#include "tools/filesystem.inl"
