#include "interaction/mousedetector.hpp"

#include "core/application.hpp"
#include "tools/debug.hpp"
#include "tools/tools.hpp"
#include "tools/event.hpp"
#include "tools/math.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace interaction;

MouseDetector::MouseDetector()
    : m_hoveredChild(nullptr)
{
}

void MouseDetector::init()
{
    const auto& resolution = Application::context().resolution;

    m_image.create(resolution.x, resolution.y);
    m_texture.create(resolution.x, resolution.y);
    m_texture.setSmooth(false);
}

void MouseDetector::draw(const Detectable& child, sf::RenderStates states)
{
    returnif (!child.detectable());

    // We are just interested in the transform
    states.texture = nullptr;
    states.shader = nullptr;

    // Finding child's info
    const ChildInfo* childInfo = nullptr;
    for (const auto& childInfoElement : m_childrenList) {
        if (childInfoElement.child == &child) {
            childInfo = &childInfoElement;
            break;
        }
    }

    massert(childInfo != nullptr, "Asking to draw a non-registered child.");

    // Drawing its representation
    sf::RectangleShape rectangleShape;
    rectangleShape.setFillColor(childInfo->color);
    rectangleShape.setSize(child.size());

    m_texture.draw(rectangleShape, states);
    m_texture.display();

    m_image = m_texture.getTexture().copyToImage();
}

void MouseDetector::draw() const
{
    auto& window = Application::context().window;

    sf::Sprite sprite(m_texture.getTexture());
    sprite.setColor({255, 255, 255, 150});
    window.draw(sprite);
}

void MouseDetector::update(const sf::Time& dt)
{
    // TODO Currently resetting each time, can be more clever
    m_texture.clear(sf::Color::Black);
}

//------------------//
//----- Events -----//

Detectable* MouseDetector::handleMouseEvent(const sf::Event& event)
{
    // Extrapolating position
    auto& window = Application::context().window;
    sf::Vector2f absPos(window.mapPixelToCoords(mousePosition(event)));
    clamp(absPos, Application::context().resolution);

    // Getting child touched if any
    Detectable* child = childFromPosition(sf::v2u(absPos));
    setHoveredChild(child);
    returnif (child == nullptr) nullptr;

    // Getting relative coordinates
    sf::Vector2f relPos = child->getInverseTransform().transformPoint(absPos);

    // Calling child callback
    if (event.type == sf::Event::MouseMoved)
        child->handleMouseMoved(relPos);
    else if (event.type == sf::Event::MouseButtonPressed)
        child->handleMouseButtonPressed(event.mouseButton.button, relPos);
    else if (event.type == sf::Event::MouseButtonReleased)
        child->handleMouseButtonReleased(event.mouseButton.button, relPos);
    else if (event.type == sf::Event::MouseWheelMoved)
        child->handleMouseWheelMoved(event.mouseWheel.delta, relPos);

    return child;
}

//-------------------------------//
//----- Children management -----//

void MouseDetector::add(Detectable* child)
{
    // FIXME With this system not reusing previously attributed colors,
    // we will possibly hit the maximum capacity in game

    // Increase current color
    increaseColor();
    massert(m_color != sf::Color::Black, "Object number exceeds core limit");
    m_childrenList.push_back({m_color, child});
    child->setMouseDetector(this);
}

//--------------------//
//----- Hovering -----//

void MouseDetector::setHoveredChild(Detectable* inHoveredChild)
{
    returnif (m_hoveredChild == inHoveredChild);

    if (m_hoveredChild == nullptr) {
        m_hoveredChild = inHoveredChild;
        return;
    }

    // Emit MouseLeft if mouse was over an other child
    sf::Event event;
    m_hoveredChild->handleMouseLeft();
    m_hoveredChild = inHoveredChild;
}

//---------------------//
//----- Low-level -----//

void MouseDetector::increaseColor()
{
    // Red
    ++m_color.r;
    returnif (m_color.r != 0);

    // Green
    ++m_color.g;
    returnif (m_color.g != 0);

    // Blue
    ++m_color.b;
}

Detectable* MouseDetector::childFromPosition(const sf::Vector2u& pos) const
{
    const auto& color = m_image.getPixel(pos.x, pos.y);

    for (const auto& childInfo : m_childrenList)
        if (childInfo.color == color)
            return childInfo.child;

    return nullptr;
}
