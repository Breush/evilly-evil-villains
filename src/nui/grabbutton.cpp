#include "nui/grabbutton.hpp"

#include "scene/graph.hpp"
#include "scene/layer.hpp"
#include "tools/tools.hpp"

using namespace nui;

GrabButton::GrabButton()
{
    setValidateCallback([this] { graph()->setGrabbable(spawnGrabbable()); });
}
