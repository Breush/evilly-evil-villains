#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>

#include <functional>

// Forward declaration

namespace scene
{
    class Layer;
}

// FIXME A mini-representation (not a view)
// of the dungeon data, might be more readable and enjoyable for the player.

namespace dungeon
{
    //! Controls and shows a specific view.

    class Minimap final : public scene::Entity
    {
        using baseClass = scene::Entity;

        //! Type used when minimap is clicked.
        using CallbackAction = std::function<void(const sf::Vector2f&)>;

    public:

        //! Constructor.
        Minimap();

        //! Default destructor.
        ~Minimap() = default;

        //! Initialize after textures are loaded.
        void init();

        std::string _name() const final { return "dungeon::Minimap"; }

        //--------------//
        //! @name Layer
        //! @{

        //! Sets the layer shown.
        void setLayer(scene::Layer& layer);

        //! @}

        //---------------//
        //! @name Action
        //! @{

        //! Sets the action on minimap clicked.
        //! This returns the coordinates of the click in the binded view.
        inline void setCallbackAction(const CallbackAction& callbackAction) { m_callbackAction = callbackAction; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void drawInternal(sf::RenderTarget& target, sf::RenderStates states) const final;
        void onTransformChanges() final;
        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;
        void refreshWindow(const config::WindowInfo& cWindow) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //---------------//
        //! @name Action
        //! @{

        //! Gets in view coordinates and inform callback if exists.
        void doAction(const sf::Vector2f& nuiPos);

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Refresh the from the layer size.
        void refreshSize();

        //! Refresh the position of the viewport.
        void refreshViewport();

        //! Refresh the representation of the view rectangle.
        void refreshViewIndicator();

        //! @}

    private:

        // View elements
        scene::Layer* m_layer = nullptr;    //!< The layer to use for the minimap.
        sf::View m_view;                    //!< The minimap view.

        // Action
        CallbackAction m_callbackAction = nullptr;  //!< Called when minimap is clicked.
        bool m_grabbing = false;                    //!< True if user maintains a click.

        // Decorum
        sf::RectangleShape m_background;            //!< The background.
        sf::RectangleShape m_layerViewIndicator;    //!< The rectangle of the view.
    };
}
