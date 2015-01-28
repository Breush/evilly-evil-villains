#include "scml/interface.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp" // mapFind

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace scml;

//-----------------------//
//----- File system -----//

FileSystem::~FileSystem()
{
    clear();
}

void FileSystem::clear()
{
    textures.clear();
    sounds.clear();
}

//----- Image

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    Textures::ID id = Application::context().textures.getID(filename);

    if (!textures.insert(std::make_pair(std::make_pair(folderID, fileID), id)).second) {
        std::cerr << "scml::FileSystem failed to load image: Loading " << filename
                  << " duplicates the folder/file id " << folderID << "/" << fileID << std::endl;
        return false;
    }

    return true;
}

std::pair<uint, uint> FileSystem::getImageDimensions(int folderID, int fileID) const
{
    // TODO There is certainly a better way to do
    const auto& texture = Application::context().textures.get(getTexture(folderID, fileID));
    return std::make_pair(texture.getSize().x, texture.getSize().y);
}

Textures::ID FileSystem::getTexture(int folderID, int fileID) const
{
    return tools::mapFind(textures, std::make_pair(folderID, fileID));
}

//----- Sounds

bool FileSystem::loadSoundFile(int folderID, int fileID, const std::string& filename)
{
    Sounds::ID id = Application::context().sounds.getID(filename);

    if (!sounds.insert(std::make_pair(std::make_pair(folderID, fileID), id)).second) {
        std::cerr << "scml::FileSystem failed to load sound: Loading " << filename
                  << " duplicates the folder/file id " << folderID << "/" << fileID << std::endl;
        return false;
    }

    return true;
}

Sounds::ID FileSystem::getSound(int folderID, int fileID) const
{
    return tools::mapFind(sounds, std::make_pair(folderID, fileID));
}

//------------------//
//----- Entity -----//

Entity::Entity()
    : scml::EntityPrototype()
{}

Entity::Entity(scml::Data* data, int entity, int animation, int key)
    : scml::EntityPrototype(data, entity, animation, key)
{}

FileSystem* Entity::setFileSystem(FileSystem* fs)
{
    FileSystem* old = file_system;
    file_system = fs;
    return old;
}

sf::RenderTarget* Entity::setScreen(sf::RenderTarget* scr)
{
    sf::RenderTarget* old = screen;
    screen = scr;
    return old;
}

void Entity::convert_to_SCML_coords(float& x, float& y, float& angle)
{
    y = -y;
    angle = 360 - angle;
}

std::pair<uint, uint> Entity::getImageDimensions(int folderID, int fileID) const
{
    return file_system->getImageDimensions(folderID, fileID);
}

// (x, y) specifies the center point of the image.  x, y, and angle are in SCML coordinate system (+x to the right, +y up, +angle counter-clockwise)
void Entity::draw_internal(int folderID, int fileID, float x, float y, float angle, float scale_x, float scale_y)
{
    y = -y;
    angle = 360 - angle;

    const auto& texture = Application::context().textures.get(file_system->getTexture(folderID, fileID));

    sf::Sprite sprite(texture);
    sprite.setOrigin(sf::v2f(texture.getSize() / 2u));
    sprite.setScale(scale_x, scale_y);
    sprite.setRotation(angle);
    sprite.setPosition(x, y);

    screen->draw(sprite);
}

void Entity::play_sound(int folderID, int fileID)
{
    Application::context().sounds.play(file_system->getSound(folderID, fileID));
}
