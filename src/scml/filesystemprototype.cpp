#include "scml/filesystemprototype.hpp"

#include "tools/debug.hpp"

#include <climits> // PATH_MAX
#include <algorithm> // find
#include <stdexcept> // runtime_error

using namespace scml;

std::string dirname(std::string source)
{
    source.erase(std::find(source.rbegin(), source.rend(), '/').base(), source.end());
    return source;
}

bool pathIsAbsolute(const std::string& path)
{
#ifdef WIN32
    if(path.size() < 3)
        return false;
    return path[1] == ':' && (path[2] == '\\' || path[2] == '/'));
#else
    if(path.size() < 1)
        return false;
    return (path[0] == '/');
#endif
    return false;
}

void FileSystemPrototype::load(scml::Data* data)
{
    if (data == NULL || data->name.size() == 0) {
        throw std::runtime_error("scml::FileSystemPrototype called with an empty data");
        return;
    }

    std::string basedir;
    if(!pathIsAbsolute(data->name))
    {
        // Create a relative directory name for the path's base
        char buf[PATH_MAX];
        snprintf(buf, PATH_MAX, "%s", data->name.c_str());

        basedir = dirname(buf);
        if (basedir.size() > 0 && basedir[basedir.size()-1] != '/')
            basedir += '/';
    }

    for (auto const& folder : data->folders)
    for (auto const& file : folder.second->files) {
        // Image
        if (file.second->type == L"image") {
            wdebug_scml_1(L"# FS: Loading image " << file.second->name);
            loadImageFile(folder.second->id, file.second->id, file.second->name);
        }
        // Sound
        else if (file.second->type == L"sound") {
            wdebug_scml_1(L"# FS: Loading sound " << file.second->name);
            loadSoundFile(folder.second->id, file.second->id, file.second->name);
        }
        // Other?
        else {
            wdebug_scml_1(L"# FS: NOT Loading unknown file type " << file.second->type
                          << " for file " << file.second->name);
        }
    }
}
