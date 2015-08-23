#include "scml/interface.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"
#include "tools/vector.hpp"
#include "tools/string.hpp"

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace scml;

//-----------------------//
//----- File system -----//

FileSystem::~FileSystem() noexcept
{
    clear();
}

void FileSystem::clear()
{
    textures.clear();
    sounds.clear();
}

//----- Images

bool FileSystem::loadImageFile(int folderID, int fileID, const std::wstring& filename)
{
    auto id = Application::context().textures.getID(toString(filename));
    auto& texture = Application::context().textures.get(id);

    if (!textures.insert(std::make_pair(std::make_pair(folderID, fileID), &texture)).second) {
        std::wcout << L"scml::FileSystem failed to load image: Loading " << filename
                   << L" duplicates the folder/file id " << folderID << "/" << fileID << std::endl;
        return false;
    }

    return true;
}

std::pair<uint, uint> FileSystem::getImageDimensions(int folderID, int fileID) const
{
    const auto& texture = getTexture(folderID, fileID);
    return std::make_pair(texture.getSize().x, texture.getSize().y);
}

sf::Texture& FileSystem::getTexture(int folderID, int fileID) const
{
    return *textures.at({folderID, fileID});
}

//----- Sounds

bool FileSystem::loadSoundFile(int folderID, int fileID, const std::wstring& filename)
{
    auto soundID = Application::context().sounds.getID(toString(filename));

    if (!sounds.insert(std::make_pair(std::make_pair(folderID, fileID), soundID)).second) {
        std::wcout << L"scml::FileSystem failed to load sound: Loading " << filename
                   << L" duplicates the folder/file id " << folderID << "/" << fileID << std::endl;
        return false;
    }

    return true;
}

const std::string& FileSystem::getSound(int folderID, int fileID) const
{
    return sounds.at({folderID, fileID});
}

//------------------//
//----- Entity -----//

Entity::Entity()
    : scml::EntityPrototype()
{}

Entity::Entity(scml::Data* data, int entity, int animation, int key)
    : scml::EntityPrototype(data, entity, animation, key)
{}

void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    y = -y;
    angle = 360 - angle;
}

std::pair<uint, uint> Entity::getImageDimensions(int folderID, int fileID) const
{
    return m_fileSystem->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system
// (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y, float alpha)
{
    y = -y;
    angle = 360 - angle;

    const auto& texture = m_fileSystem->getTexture(folderID, fileID);

    m_sprite.setTexture(texture, true);
    m_sprite.setOrigin(sf::v2f(texture.getSize() / 2u));
    m_sprite.setScale(scale_x, scale_y);
    m_sprite.setRotation(angle);
    m_sprite.setPosition(x, y);
    m_sprite.setColor({255u, 255u, 255u, static_cast<uint8>(alpha * 255.f)});

    m_target->draw(m_sprite);
}

void Entity::play_sound(int folderID, int fileID)
{
    Application::context().sounds.play(m_fileSystem->getSound(folderID, fileID));
}
