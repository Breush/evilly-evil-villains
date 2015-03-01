#include "nui/grabbutton.hpp"

#include "scene/graph.hpp"

using namespace nui;

GrabButton::GrabButton()
{
}

//-----------------//
//----- Mouse -----//

void GrabButton::handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f&, const sf::Vector2f&)
{
    if (button == sf::Mouse::Left)
      graph()->setGrabbable(std::move(spawnGrabbable()));
}

//------------------//
//----- Action -----//
