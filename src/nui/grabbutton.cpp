#include "nui/grabbutton.hpp"

#include "scene/graph.hpp"
#include "scene/layer.hpp"
#include "tools/tools.hpp"

using namespace nui;

//------------------//
//----- Events -----//

bool GrabButton::handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f&, const sf::Vector2f&)
{
    returnif (button != sf::Mouse::Left) false;
    graph()->setGrabbable(spawnGrabbable());
    return true;
}
