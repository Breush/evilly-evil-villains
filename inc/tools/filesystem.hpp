#pragma once

#include "tools/string.hpp" // toString

#include <fstream>

#if defined(__WIN32__)
    // Windows
    #include <windows.h>
#else
    // POSIX
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <dirent.h>
#endif

//! Quick tool to extract file extension.
inline std::string fileExtension(const std::string& file)
{
  return file.substr(file.find_last_of(".") + 1u);
}

//! Returns true if directory was successfully created.
inline bool createDirectory(const std::wstring& directory)
{
#if defined(__WIN32__)
    // Windows
    return CreateDirectoryW(directory.c_str(), 0) != 0;

#else
    // POSIX
    return mkdir(toString(directory).c_str(), S_IRWXU | S_IRWXG | S_IRWXO) == 0;
#endif
}

//! Returns true if file exists.
inline bool fileExists(const std::string& filename)
{
    std::ifstream fstr(filename);
    return fstr;
}

inline bool fileExists(const std::wstring& filename)
{
    return fileExists(toString(filename));
}

//! Stores file information.
struct FileInfo
{
    std::string name;       //!< File name.
    std::string fullName;   //!< Full file name (with directory). Note: may not be absolute.
    bool isDirectory;       //!< True if directory.
};

//! List all files and directories.
//! If recursive, be sure that there is no loop with symlink.
inline std::vector<FileInfo> listFiles(const std::string& directory, bool recursive = false)
{
    std::vector<FileInfo> filesInfo;

#if defined(__WIN32__)
    // Windows
    HANDLE dir;
    WIN32_FIND_DATA file_data;

    if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
        return; /* No files found */

    do {
        const string file_name = file_data.cFileName;
        const string full_file_name = directory + "/" + file_name;
        const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;

        if (file_name[0] == '.')
            continue;

        if (is_directory)
            continue;

        filesInfo.push_back(full_file_name);
    } while (FindNextFile(dir, &file_data));

    FindClose(dir);

#else
    // POSIX
    dirent *ent;
    class stat st;
    DIR* dir = opendir(directory.c_str());

    while ((ent = readdir(dir)) != nullptr) {
        const std::string fileName = ent->d_name;
        const std::string fullFileName = directory + "/" + fileName;

        // Hidden file or ./.. directories
        if (fileName[0] == '.')
            continue;

        // No correct access
        if (stat(fullFileName.c_str(), &st) == -1)
            continue;

        // Do recursive if wanted
        const bool isDirectory = (st.st_mode & S_IFDIR) != 0;
        if (isDirectory && recursive) {
            std::vector<FileInfo> recFilesInfo = listFiles(fullFileName, true);
            filesInfo.insert(std::end(filesInfo), std::begin(recFilesInfo), std::end(recFilesInfo));
        }

        filesInfo.push_back({fileName, fullFileName, isDirectory});
    }

    closedir(dir);
#endif

    return filesInfo;
}
