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

inline std::string fileExtension(const std::string& file)
{
  return file.substr(file.find_last_of(".") + 1u);
}

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

inline bool fileExists(const std::string& filename)
{
    std::ifstream fstr(filename);
    return fstr;
}

inline bool fileExists(const std::wstring& filename)
{
    return fileExists(toString(filename));
}

inline std::vector<FileInfo> listFiles(const std::string& directory, bool recursive)
{
    std::vector<FileInfo> filesInfo;

#if defined(__WIN32__)
    // Windows
    HANDLE dir;
    WIN32_FIND_DATA fileData;

    // No files found
    if ((dir = FindFirstFile((directory + "/*").c_str(), &fileData)) == INVALID_HANDLE_VALUE)
        return filesInfo;

    do {
        const std::string fileName = fileData.cFileName;
        const std::string fullFileName = directory + "/" + fileName;

        // Hidden file or ./.. directories
        if (fileName[0] == '.')
            continue;

        // Do recursive if wanted
        const bool isDirectory = (fileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
        if (isDirectory && recursive) {
            std::vector<FileInfo> recFilesInfo = listFiles(fullFileName, true);
            filesInfo.insert(std::end(filesInfo), std::begin(recFilesInfo), std::end(recFilesInfo));
        }

        filesInfo.push_back({fileName, fullFileName, isDirectory});
    } while (FindNextFile(dir, &fileData));

    FindClose(dir);

#else
    // POSIX
    dirent *ent;
    class stat st;
    DIR* dir = opendir(directory.c_str());

    if (dir == nullptr)
        throw std::runtime_error("Directory " + directory + " does not exist or is unaccessible.");

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
