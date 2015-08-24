#include "nui/reactimage.hpp"

#include "core/application.hpp"
#include "tools/math.hpp"
#include "tools/tools.hpp"
#include "tools/string.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <pugixml/pugixml.hpp>
#include <string>

using namespace nui;

ReactImage::ReactImage()
    : baseClass()
    , m_mouseLeftDeselect(true)
{
    // Sprite
    addPart(&m_sprite);
    m_sprite.setPosition(0.f, 0.f);
    m_sprite.setOrigin(0.f, 0.f);
}

//-------------------//
//----- Routine -----//

void ReactImage::updateRoutine(const sf::Time& dt)
{
    m_activeReactChanged = false;
}

//------------------//
//----- Events -----//

void ReactImage::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    returnif (button != sf::Mouse::Left);
    returnif (m_activeReact == nullptr);

    const auto& callback = m_activeReact->callback;

    // Maybe callback is not set
    if (callback == nullptr) {
        Application::context().sounds.play("refuse");
        return;
    }

    // Accept callback
    Application::context().sounds.play("accept");
    callback();
}

void ReactImage::handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos)
{
    // Looking for active react
    for (auto& react : m_reacts) {
        if (react.rect.contains(sf::v2i(mousePos))) {
            setActiveReact(react.key);
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

//-------------------//
//----- Control -----//

void ReactImage::loadFromFile(const std::string& file)
{
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    const auto& reactImageNode = doc.child(L"reactimage");

    // Get texture if any
    std::wstring textureString = reactImageNode.attribute(L"texture").as_string();
    if (!textureString.empty())
        setTexture(toString(textureString));

    // Adding children
    for (const auto& reactNode : reactImageNode.children()) {
        std::wstring key = reactNode.attribute(L"key").as_string();
        auto left = reactNode.attribute(L"left").as_int();
        auto top = reactNode.attribute(L"top").as_int();
        auto width = reactNode.attribute(L"width").as_int();
        auto height = reactNode.attribute(L"height").as_int();
        addReact(std::move(key), {left, top, width, height});
    }
}

//-------------------//
//----- Texture -----//

void ReactImage::setTexture(const std::string& id)
{
    sf::Texture& texture = Application::context().textures.get(id);
    m_image.setTexture(texture);

    // Getting size and reconfiguring visual
    m_imageRect = m_image.getLocalBounds();
    sf::Vector2f imageSize(m_imageRect.left + m_imageRect.width, m_imageRect.top + m_imageRect.height);

    // Creating target
    m_buffer.create(imageSize.x, imageSize.y);
    m_buffer.setSmooth(true);

    // Setting final size
    setSize(imageSize);
}

//----------------------------//
//----- React management -----//

void ReactImage::addReact(std::wstring key, sf::IntRect rect, const Callback callback)
{
    massert(!key.empty(), "Cannot add react with empty key.");

    // Automatic length detection
    if (rect.width < 0.f)  rect.width  = m_imageRect.width  - rect.left;
    if (rect.height < 0.f) rect.height = m_imageRect.height - rect.top;

    // Finally add new react
    ReactInfo reactInfo;
    reactInfo.key = std::move(key);
    reactInfo.rect = std::move(rect);
    reactInfo.callback = callback;
    m_reacts.push_back(std::move(reactInfo));

    // Note: There can be a bug if you add reacts after selecting one.
    // As the std::vector can move the memory around when adding,
    // so the pointer to ReactInfo becomes invalid.
}

void ReactImage::setReactCallback(const std::wstring& key, Callback callback)
{
    auto found = std::find_if(m_reacts, [&key](const ReactInfo& reactInfo) { return reactInfo.key == key; });
    wassert(found != m_reacts.end(), L"Cannot set react with key '" << key << L"' callback, because key is not found.");
    found->callback = callback;
}

void ReactImage::setActiveReact(const std::wstring& key)
{
    // Do not change if already set
    returnif (m_activeReact != nullptr && m_activeReact->key == key);

    // Does key exists?
    auto found = std::find_if(m_reacts, [&key](const ReactInfo& reactInfo) { return reactInfo.key == key; });
    wassert(found != m_reacts.end(), L"Cannot set react with key '" << key << L"' active, because key is not found.");

    // Set and refresh
    m_activeReact = &(*found);
    m_activeReactChanged = true;
    refreshBuffer();

    Application::context().sounds.play("select");
}

//--------------------------------//
//----- Rectangles selection -----//

void ReactImage::selectImageRect()
{
    m_image.setColor(sf::Color::White);
    m_image.setTextureRect(sf::IntRect(m_imageRect));
    m_image.setPosition(0.f, 0.f);
}

void ReactImage::selectActiveRect()
{
    // Check for something active
    returnif (m_activeReact == nullptr);

    // Choose color
    if (m_activeReact->callback != nullptr) m_image.setColor(sf::Color::Red);
    else m_image.setColor({200, 125, 125});

    // Move Visible rect
    const auto& activeRect = m_activeReact->rect;
    m_image.setTextureRect(activeRect);
    m_image.setPosition(activeRect.left, activeRect.top);
}

//------------------------------------//
//----- Internal changes update  -----//

void ReactImage::refreshBuffer()
{
    returnif (m_image.getTexture() == nullptr);

    m_buffer.clear(sf::Color::Transparent);

    // Paint whole image
    selectImageRect();
    m_buffer.draw(m_image);

    // Paint active rect
    if (m_activeReact != nullptr) {
        selectActiveRect();
        m_buffer.draw(m_image);
    }

    // Giving buffer to window
    m_buffer.display();
    m_sprite.setTexture(m_buffer.getTexture());
}
