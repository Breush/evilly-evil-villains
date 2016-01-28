#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/View.hpp>
#include <ltbl/lighting/LightSystem.h>

#include <functional>

// Forward declarations

namespace config
{
    class NUIGuides;
    class WindowInfo;
}

namespace scene
{
    class LightEmitter;
    class LightNormals;

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

        friend class LightEmitter;
        friend class LightNormals;

    public:

        //! Constructor.
        Layer();

        //! Default destructor.
        ~Layer() = default;

        //! Initialization.
        void init(Graph* graph);

        //----------------//
        //! @name Routine
        //! @{

        //! Recursively update on all entities of the layer.
        void update(const sf::Time& dt, const float factor);

        //! Set target to layer view and draw layer recursively.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Reset views to current screen status.
        void refreshWindow(const config::WindowInfo& cWindow);

        //! Refresh NUI appearance.
        void refreshNUI(const config::NUIGuides& cNUI);

        //! Refresh from size change.
        void refreshSize();

        //! @}

        //-----------------//
        //! @name Lighting
        //! @{

        //! Enable the lighting rendering over the layer.
        //! All entities with the corresponding components will influence this.
        void turnLights(bool on);

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

    protected:

        //-----------------//
        //! @name Lighting
        //! @{

        //! Access the light system.
        ltbl::LightSystem& lightSystem() { return m_lightSystem; }

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the basic view parameters.
        void refreshBasicView();

        //! Recreate the light system if needed.
        void refreshLightSystem();

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

        // Drawing
        mutable sf::RenderTexture m_tmpTarget;  //!< Temporary target to draw.
        sf::View m_basicView;                   //!< The view used to render.
        sf::View m_internView;                  //!< The view, but with a viewport relative to the layer size, not the screen.

        // Lighting
        bool m_lightsOn = false;                        //!< Whether or not the light system is active for this layer.
        mutable ltbl::LightSystem m_lightSystem;        //!< The light system.
        sf::RenderStates m_lightRenderStates;           //!< The render states with correct blending for the lighting.
        const sf::Texture* m_penumbraTexture = nullptr; //!< The penumbra texture.
        sf::Shader* m_lightOverShapeShader = nullptr;   //!< The light over shape shader.
        sf::Shader* m_unshadowShader = nullptr;         //!< The unshadow shader.
        sf::Shader* m_normalsShader = nullptr;          //!< The normals shader.

        bool m_lightDebugFirstTime = true;  // FIXME Debug thing.

    };
}
