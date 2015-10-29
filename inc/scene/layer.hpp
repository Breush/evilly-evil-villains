#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/View.hpp>

#include <functional>

// Forward declarations

namespace config
{
    class NUIGuides;
    class WindowInfo;
}

namespace scene
{
    //! A layer root entity.

    class LayerRoot final : public Entity
    {
    public:

        //! Constructor.
        LayerRoot();

        std::string _name() const final { return "LayerRoot"; }
    };

    //! A layer within the Graph.
    /*!
     *  A layer has a fixed size and acts on a view.
     *  If set manipulable, it reacts to grabbing and zooming.
     *  It is also the container for the initial node in the Graph layer.
     */

    class Layer final : public sf::Drawable
    {
        //! General callback function.
        using Callback = std::function<void()>;

    public:

        //! Default constructor.
        Layer() = default;

        //! Initialization.
        void init(Graph* graph);

        //----------------//
        //! @name Routine
        //! @{

        //! Recursively update on all entities of the layer.
        void update(const sf::Time& dt);

        //! Set target to layer view and draw layer recursively.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Reset views to current screen status.
        void refreshWindow(const config::WindowInfo& cWindow);

        //! Refresh NUI appearance.
        void refreshNUI(const config::NUIGuides& cNUI);

        //! Refresh from size change.
        void refreshSize();

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

        //! Sets the viewport.
        //! If not used, the default is resolution-wide.
        void setViewport(const sf::FloatRect& viewport);

        //! Set the size of the view.
        void setViewSize(const sf::Vector2f& viewSize);

        //! Fixes the view center relatively to current layer size.
        //! 0.f <= relativeCenter.x <= 1.f and 0.f <= relativeCenter.y <= 1.f
        void setRelativeCenter(const sf::Vector2f& relativeCenter);

        //! Returns the first entity at specific position, or nullptr if none.
        //! Even if not found, viewPos is set to the position within the layer view.
        Entity* entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos);

        //! Called whenever manipuability status changed.
        void refreshManipulability();

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

        //------------------------//
        //! @name Callback system
        //! @{

        //! Sets a callback function to call whenever the size changes.
        void callOnSizeChanges(const Callback& callback) { m_onSizeChangesCallback = callback; }

        //! Sets a callback function to call whenever the view changes.
        void callOnViewChanges(const Callback& callback) { m_onViewChangesCallback = callback; }

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the layer needs to control the view.
        PARAMGSU(bool, m_manipulable, manipulable, setManipulable, refreshManipulability)

        //! The size of the region that can be displayed.
        PARAMGSU(sf::Vector2f, m_size, size, setSize, refreshSize)

        //! The depth of the layer, only used with Scene.
        PARAMGS(float, m_depth, depth, setDepth)

        //! @}

    private:

        //! The entity, as a layer holds the first node.
        LayerRoot m_root;

        //! The view, as the layer controls its the displayed region.
        sf::View m_view;

        // Viewports
        bool m_ownViewport = false; //!< Whether or not we use a provided viewport.

        //! Called whenever the size changes.
        Callback m_onSizeChangesCallback = nullptr;

        //! Called whenever the view changes.
        Callback m_onViewChangesCallback = nullptr;
    };
}
