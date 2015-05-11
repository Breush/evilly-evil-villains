#include "nui/grabbutton.hpp"

#include "scene/graph.hpp"
#include "scene/layer.hpp"

using namespace nui;

//------------------//
//----- Events -----//

void GrabButton::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    if (button == sf::Mouse::Left)
      graph()->setGrabbable(spawnGrabbable());
}
