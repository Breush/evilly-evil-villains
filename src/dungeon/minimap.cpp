#include "dungeon/minimap.hpp"

#include "core/application.hpp"
#include "scene/layer.hpp"
#include "tools/vector.hpp"
#include "tools/tools.hpp"

using namespace dungeon;

Minimap::Minimap()
{
    // Decorum
    addPart(&m_background);
    m_background.setOutlineThickness(1.f);
    m_background.setOutlineColor(sf::Color::White);
    m_background.setFillColor({255u, 255u, 255u, 150u});

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void Minimap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    baseClass::draw(target, states);
    returnif (m_layer == nullptr);

    auto previousView = target.getView();

    // Recompute position,have to do it here because target is needed...
    // TODO Have a flag to not recompute is nothing happened?
    // Or move this to a refreshViewport function called when position/size changed.
    const auto& screenSize = Application::context().screenSize;
    const sf::FloatRect rect{getPosition().x, getPosition().y, size().x, size().y};
    auto screenRect = tools::mapRectCoordsToPixel(target, previousView, rect);
    const_cast<Minimap*>(this)->m_view.setViewport(screenRect / screenSize);

    // Draw the minimap
    target.setView(m_view);
    target.draw(m_layer->root());
    target.setView(previousView);
}

void Minimap::onSizeChanges()
{
    m_background.setSize(size());
}

void Minimap::refreshDisplay()
{
    baseClass::refreshDisplay();
}

//-----------------//
//----- Layer -----//

void Minimap::setLayer(const scene::Layer& layer)
{
    m_layer = &layer;

    setSize(m_layer->size() / 15.f);
    m_view.setSize(m_layer->size());
    m_view.setCenter(m_layer->size() / 2.f);
}
