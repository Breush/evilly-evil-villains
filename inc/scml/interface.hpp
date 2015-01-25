#pragma once

#include "scml/filesystemprototype.hpp"
#include "scml/entityprototype.hpp"

#include <SFML/Graphics/Texture.hpp>

// To render with SFML

namespace SCML
{

class FileSystem : public FileSystemPrototype
{
public:

    // Folder, File
    std::map<std::pair<int, int>, sf::Texture*> images;

    virtual ~FileSystem();

    bool loadImageFile(int folderID, int fileID, const std::string& filename) override;
    void clear() override;
    std::pair<uint, uint> getImageDimensions(int folderID, int fileID) const override;

    sf::Texture* getImage(int folderID, int fileID) const;
};

class Entity : public EntityPrototype
{
public:

    Entity();
    Entity(SCML::Data* data, int entity, int animation = 0, int key = 0);

    FileSystem* setFileSystem(FileSystem* fs);
    sf::RenderTarget* setScreen(sf::RenderTarget* scr);

    void convert_to_SCML_coords(float& x, float& y, float& angle) override;
    std::pair<uint, uint> getImageDimensions(int folderID, int fileID) const override;
    void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y) override;

private:
    FileSystem* file_system;
    sf::RenderTarget* screen;
};

}
