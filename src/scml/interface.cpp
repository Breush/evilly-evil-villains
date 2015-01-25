#include "scml/interface.hpp"

#include "tools/tools.hpp" // mapFind

#include <SFML/Graphics.hpp>
#include <iostream>

using namespace SCML;

//-----------------------//
//----- File system -----//

// TODO Remove: Use textureHandler instead

FileSystem::~FileSystem()
{
    clear();
}

bool FileSystem::loadImageFile(int folderID, int fileID, const std::string& filename)
{
    sf::Texture* img = new sf::Texture;
    img->loadFromFile(filename);

    returnif (img == nullptr) false;
    img->setSmooth(true);

    if (!images.insert(std::make_pair(std::make_pair(folderID, fileID), img)).second)
    {
        std::cerr << "SCML::FileSystem failed to load image: Loading " << filename
                  << " duplicates the folder/file id " << folderID << "/" << fileID << std::endl;
        delete img;
        return false;
    }
    return true;
}

void FileSystem::clear()
{
    typedef std::pair<int,int> pair_type;
    for(auto& image : images)
        delete image.second;
    images.clear();
}

std::pair<uint, uint> FileSystem::getImageDimensions(int folderID, int fileID) const
{
    sf::Texture* img = tools::mapFind(images, std::make_pair(folderID, fileID));
    if(img == NULL)
        return std::make_pair(0,0);
    return std::make_pair(img->getSize().x, img->getSize().y);
}

sf::Texture* FileSystem::getImage(int folderID, int fileID) const
{
    return tools::mapFind(images, std::make_pair(folderID, fileID));
}

//------------------//
//----- Entity -----//

Entity::Entity()
    : SCML::EntityPrototype()
{}

Entity::Entity(SCML::Data* data, int entity, int animation, int key)
    : SCML::EntityPrototype(data, entity, animation, key)
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

    sf::Texture* img = file_system->getImage(folderID, fileID);
    returnif (img == NULL);

    // FIXME It's recreating a sprite each time
    sf::Sprite sprite(*img);
    sprite.setOrigin(img->getSize().x/2, img->getSize().y/2);
    sprite.setScale(scale_x, scale_y);
    sprite.setRotation(angle);
    sprite.setPosition(x, y);

    //std::cout << "Pos " << x << " " << y << std::endl;
    //std::cout << "Scale " << scale_x << " " << scale_y << std::endl;
    //std::cout << "Origine " << img->getSize().x/2 << " " << img->getSize().y/2 << std::endl;

    screen->draw(sprite);
}
