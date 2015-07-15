#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/View.hpp>

namespace scene
{
    //! A layer within the Graph.
    /*!
     *  A layer has a fixed size and acts on a view.
     *  If set manipulable, it reacts to grabbing and zooming.
     *  It is also the container for the initial node in the Graph layer.
     */

    class Layer final : public sf::Drawable
    {
    public:

        //! Deleted default constructor.
        Layer() = delete;

        //! Constructor with original graph.
        Layer(Graph* graph);

        //----------------//
        //! @name Routine
        //! @{

        //! Recursively update on all entities of the layer.
        void update(const sf::Time& dt);

        //! Set target to layer view and draw layer recursively.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Reset the view to current screen status.
        void refreshDisplay();

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! The first entity in the layer.
        inline Entity& root() { return m_root; }

        //! The first (const) entity in the layer.
        inline const Entity& root() const { return m_root; }

        //! The view as previously set.
        inline const sf::View& view() const { return m_view; }

        //! @}

        //--------------------------//
        //! @name View manipulation
        //! @{

        //! Set the size of the view.
        void setViewSize(const sf::Vector2f& viewSize);

        //! Fixes the view center relatively to current layer size.
        //! 0.f <= relativeCenter.x <= 1.f and 0.f <= relativeCenter.y <= 1.f
        void setRelativeCenter(const sf::Vector2f& relativeCenter);

        //! Returns the first entity at specific position, or nullptr if none.
        //! Even if not found, viewPos is set to the position within the layer view.
        Entity* entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos);

        //! @}

        //---------------------//
        //! @name Visible rect
        //! @{

        //! The visible rectangle, so that if view size is bigger
        //! than layer size, the visible rect is all that can be seen.
        sf::FloatRect visibleRect() const;

        //! Set size and position of an entity to visible rect coordinates.
        void fitToVisibleRect(scene::Entity& entity);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the layer needs to control the view.
        PARAMGSU(bool, m_manipulable, manipulable, setManipulable, refreshDisplay)

        //! The size of the region that can be displayed.
        PARAMGSU(sf::Vector2f, m_size, size, setSize, refreshDisplay)

        //! The depth of the layer, only used with Scene.
        PARAMGS(float, m_depth, depth, setDepth)

        //! @}

    private:

        //! The entity, as a layer holds the first node.
        Entity m_root;

        //! The view, as the layer controls its the displayed region.
        sf::View m_view;
    };
}
