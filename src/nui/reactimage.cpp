#include "core/application.hpp"
#include "nui/reactimage.hpp"
#include "nui/uicore.hpp"
#include "resources/soundplayer.hpp"
#include "tools/math.hpp"
#include "tools/debug.hpp"
#include "tools/platform-fixes.hpp" // std::stoi

#include <SFML/Graphics/RenderWindow.hpp>
#include <pugixml.hpp>
#include <string>

using namespace nui;

ReactImage::ReactImage()
    : baseClass()
    , m_mouseLeftDeselect(true)
    , m_activeRect(nullptr)
    , m_react()
{
    addPart(&m_sprite);
}

void ReactImage::init()
{
}

// When position changed, recompute position of sprite
void ReactImage::update()
{
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
    m_sprite.setPosition(0.f, 0.f);
    m_sprite.setOrigin(0.f, 0.f);
    setStatus(true);
}

//-------------------//
//----- Texture -----//

void ReactImage::setImageTexture(Textures::ID id)
{
    sf::Texture& texture = Application::context().textures.get(id);
    m_image.setTexture(texture);

    // Getting size and reconfiguring visual
    m_imageRect = m_image.getLocalBounds();
    setSize({m_imageRect.left + m_imageRect.width, m_imageRect.top + m_imageRect.height});

    // Creating target
    m_target.create(size().x, size().y);
    m_target.setSmooth(true);

    update();
}

void ReactImage::setImageShader(Shaders::ID id)
{
    setPartShader(&m_sprite, id);
}

//----------------------------//
//----- React management -----//

void ReactImage::addReactFromFile(const std::string& file)
{
    pugi::xml_document doc;
    doc.load_file(file.c_str());

    // Adding children
    for (const auto& react : doc.child(L"reactimage").children()) {
        std::wstring key = react.attribute(L"key").as_string();
        auto left = react.attribute(L"left").as_float();
        auto top = react.attribute(L"top").as_float();
        auto width = react.attribute(L"width").as_float();
        auto height = react.attribute(L"height").as_float();
        addReact(key, {left, top, width, height});
    }
}

void ReactImage::addReact(const std::wstring& key, sf::FloatRect rect, const Callback callback)
{
    massert(!key.empty(), "Cannot add react with empty key");

    // Automatic length detection
    if (rect.width < 0.f)  rect.width = m_imageRect.width - rect.left;
    if (rect.height < 0.f) rect.height = m_imageRect.height - rect.top;

    // Finally add new react
    m_reacts[key] = {rect, callback};
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

    update();
}

void ReactImage::activateReact(const std::wstring& key)
{
    // Do not change if already set
    if (m_react == key)
        return;

    setActiveReact(key);
    Application::context().sounds.play(Sounds::NUI_SELECT);
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

void ReactImage::handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos)
{
    switch (event.type) {
    case sf::Event::MouseButtonPressed:
        handleMousePressed(event, relPos);
        break;
    case sf::Event::MouseMoved:
        handleMouseMoved(event, relPos);
        break;
    case sf::Event::MouseLeft:
        handleMouseLeft();
        break;
    default:
        break;
    }
}

void ReactImage::handleMousePressed(const sf::Event& event, const sf::Vector2f& relPos)
{
    // Just manage left click
    if (event.mouseButton.button != sf::Mouse::Left)
        return;

    // Maybe callback is not set
    if (m_reacts[m_react].callback == nullptr) {
        Application::context().sounds.play(Sounds::NUI_REFUSE);
        return;
    }

    // Accept callback
    Application::context().sounds.play(Sounds::NUI_ACCEPT);
    m_reacts[m_react].callback();
}

void ReactImage::handleMouseMoved(const sf::Event& event, const sf::Vector2f& relPos)
{
    // Getting mouse position relatively to image
    sf::Vector2f pos = getInverseTransform().transformPoint(relPos);

    // Looking for active react
    for (auto& react : m_reacts) {
        if (isInsideRect(pos, react.second.rect)) {
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
