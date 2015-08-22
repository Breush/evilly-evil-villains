#pragma once

#include "scml/filesystemprototype.hpp"
#include "scml/entityprototype.hpp"

#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

namespace scml
{

class FileSystem : public FileSystemPrototype
{
public:

    virtual ~FileSystem() noexcept;
    void clear() override;

    bool loadImageFile(int folderID, int fileID, const std::wstring& filename) override;
    bool loadSoundFile(int folderID, int fileID, const std::wstring& filename) override;

    std::pair<uint, uint> getImageDimensions(int folderID, int fileID) const override;

    sf::Texture& getTexture(int folderID, int fileID) const;
    const std::string& getSound(int folderID, int fileID) const;

private:
    // Folder, File
    std::map<std::pair<int, int>, sf::Texture*> textures;
    std::map<std::pair<int, int>, std::string> sounds;
};

class Entity : public EntityPrototype
{
public:

    Entity();
    Entity(scml::Data* data, int entity, int animation = 0, int key = 0);

    void setFileSystem(FileSystem* fileSystem) { m_fileSystem = fileSystem; }
    void setScreen(sf::RenderTarget* target) { m_target = target; }

    void convert_to_SCML_coords(float& x, float& y, float& angle) override;
    std::pair<uint, uint> getImageDimensions(int folderID, int fileID) const override;
    void draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y) override;

    void play_sound(int folderID, int fileID) override;

private:
    FileSystem* m_fileSystem;
    sf::RenderTarget* m_target;

    sf::Sprite m_sprite;
};

}
