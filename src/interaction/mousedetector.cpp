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

void MouseDetector::draw() const
{
    returnif (m_hoveredChild == nullptr);

    auto& window = Application::context().window;

    sf::RectangleShape rectangleShape;
    rectangleShape.setFillColor({255, 0, 0, 150});
    rectangleShape.setSize(m_hoveredChild->size());
    rectangleShape.setPosition(m_hoveredChild->getPosition());
    rectangleShape.setRotation(m_hoveredChild->getRotation());
    rectangleShape.setScale(m_hoveredChild->getScale());
    rectangleShape.setOrigin(m_hoveredChild->getOrigin());

    window.draw(rectangleShape);
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
    Detectable* child = childFromPosition(absPos);
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
    // Adding child to the list
    m_children.push_back(child);
    m_children.sort([](Detectable* a, Detectable* b) { return a->zDepth() <= b->zDepth(); });
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

Detectable* MouseDetector::childFromPosition(const sf::Vector2f& mousePos) const
{
    for (const auto& child : m_children) {
        if (child->detectable()) {
            sf::FloatRect localBounds(0.f, 0.f, child->size().x, child->size().y);
            if (localBounds.contains(child->getInverseTransform().transformPoint(mousePos)))
                return child;
        }
    }

    return nullptr;
}
