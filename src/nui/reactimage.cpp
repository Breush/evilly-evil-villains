#include "nui/reactimage.hpp"

#include "core/application.hpp"
#include "resources/identifiers.hpp"
#include "tools/math.hpp"
#include "tools/debug.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <pugixml/pugixml.hpp>
#include <string>

using namespace nui;

ReactImage::ReactImage()
    : baseClass()
    , m_mouseLeftDeselect(true)
    , m_reactChanged(true)
{
    // Sprite
    m_sprite.setPosition(0.f, 0.f);
    m_sprite.setOrigin(0.f, 0.f);
}

void ReactImage::updateRoutine(const sf::Time& dt)
{
    m_reactChanged = false;
}

void ReactImage::refreshImage()
{
    returnif (m_image.getTexture() == nullptr);

    clearParts();
    addPart(&m_sprite);

    m_target.clear(sf::Color::Transparent);

    // Paint whole image
    selectImageRect();
    m_target.draw(m_image);

    // Paint active rect
    if (!m_react.empty()) {
        selectActiveRect();
        m_target.draw(m_image);
    }

    // Giving buffer to window
    m_target.display();
    m_sprite.setTexture(m_target.getTexture());
}

//-------------------//
//----- Texture -----//

void ReactImage::setImageTexture(TextureID id)
{
    sf::Texture& texture = Application::context().textures.get(id);
    m_image.setTexture(texture);

    // Getting size and reconfiguring visual
    m_imageRect = m_image.getLocalBounds();
    sf::Vector2f imageSize(m_imageRect.left + m_imageRect.width, m_imageRect.top + m_imageRect.height);

    // Creating target
    m_target.create(imageSize.x, imageSize.y);
    m_target.setSmooth(true);

    // Setting final size
    setSize(imageSize);
}

//----------------------------//
//----- React management -----//

void ReactImage::addReactFromFile(const std::string& file)
{
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& reactImageNode = doc.child(L"reactimage");

    // Get texture if any
    std::wstring textureString = reactImageNode.attribute(L"texture").as_string();
    if (!textureString.empty())
        setImageTexture(Application::context().textures.getID(textureString));

    // Adding children
    for (const auto& reactNode : reactImageNode.children()) {
        std::wstring key = reactNode.attribute(L"key").as_string();
        auto left = reactNode.attribute(L"left").as_float();
        auto top = reactNode.attribute(L"top").as_float();
        auto width = reactNode.attribute(L"width").as_float();
        auto height = reactNode.attribute(L"height").as_float();
        addReact(std::move(key), {left, top, width, height});
    }
}

void ReactImage::addReact(const std::wstring& key, sf::FloatRect rect, const Callback callback)
{
    massert(!key.empty(), "Cannot add react with empty key");

    // Automatic length detection
    if (rect.width < 0.f)  rect.width  = m_imageRect.width  - rect.left;
    if (rect.height < 0.f) rect.height = m_imageRect.height - rect.top;

    // Finally add new react
    m_reacts[key] = {std::move(rect), callback};
}

void ReactImage::setReactCallback(const std::wstring& key, Callback callback)
{
    wassert(m_reacts.find(key) != m_reacts.end(), L"Key " << key << L" not found");
    m_reacts[key].callback = callback;
}

void ReactImage::setActiveReact(const std::wstring& key)
{
    wassert(key.empty() || m_reacts.find(key) != m_reacts.end(), L"Key " << key << L" not found");

    if (!key.empty()) m_activeRect = &m_reacts[key].rect;
    m_react = key;
    m_reactChanged = true;

    refreshImage();
}

void ReactImage::activateReact(const std::wstring& key)
{
    // Do not change if already set
    if (m_react == key)
        return;

    setActiveReact(key);
    Application::context().sounds.play(SoundID::NUI_SELECT);
}

//--------------------------------//
//----- Rectangles selection -----//

void ReactImage::selectImageRect()
{
    m_image.setColor(sf::Color::White);
    m_image.setTextureRect(sf::IntRect(m_imageRect));
    m_image.setOrigin(0.f, 0.f);
}

void ReactImage::selectActiveRect()
{
    // Check for something active
    if (m_react.empty())
        return;

    // Choose color
    if (m_reacts[m_react].callback != nullptr)
        m_image.setColor(sf::Color::Red);
    else
        m_image.setColor({200, 125, 125});

    // Move Visible rect
    m_image.setTextureRect(sf::IntRect(*m_activeRect));
    m_image.setOrigin(-m_activeRect->left, -m_activeRect->top);
}

//------------------------//
//----- Mouse events -----//

void ReactImage::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left);

    // Maybe callback is not set
    if (m_reacts[m_react].callback == nullptr) {
        Application::context().sounds.play(SoundID::NUI_REFUSE);
        return;
    }

    // Accept callback
    Application::context().sounds.play(SoundID::NUI_ACCEPT);
    m_reacts[m_react].callback();
}

void ReactImage::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Looking for active react
    for (auto& react : m_reacts) {
        if (react.second.rect.contains(mousePos)) {
            activateReact(react.first);
            return;
        }
    }

    // Nothing found means no active react
    handleMouseLeft();
}

void ReactImage::handleMouseLeft()
{
    if (m_mouseLeftDeselect)
        setActiveReact(L"");
}
