#pragma once

#include "scml/data.hpp"
#include "tools/int.hpp"

namespace scml
{

// A storage class for images in a renderer-specific format (to be inherited)

class FileSystemPrototype
{
public:

    virtual ~FileSystemPrototype() {}

    /*! \brief Loads all images referenced by the given SCML data.
     * \param data SCML data object
     */
    virtual void load(scml::Data* data);

    /*! \brief Loads an image from a file and stores it so that the folderID and fileID can be used to reference the image.
     * \param folderID Integer folder ID
     * \param fileID Integer file ID
     * \param filename Path of the image file
     * \return true on success, false on failure
     */
    virtual bool loadImageFile(int folderID, int fileID, const std::string& filename) = 0;
    virtual bool loadSoundFile(int folderID, int fileID, const std::string& filename) = 0;

    /*! \brief Cleans up all memory used by the FileSystem to store images, resetting it to an empty state.
     */
    virtual void clear() = 0;

    /*! \brief Gets the dimensions of an image
     * \param folderID Integer folder ID
     * \param fileID Integer file ID
     * \return A pair consisting of the width and height of the image.  Returns (0,0) on error.
     */
    virtual std::pair<uint, uint> getImageDimensions(int folderID, int fileID) const = 0;
};

}

