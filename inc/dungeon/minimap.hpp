#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/View.hpp>

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

    public:

        //! Constructor.
        Minimap();

        //! Default destructor.
        ~Minimap() = default;

        //--------------//
        //! @name Layer
        //! @{

        //! Sets the layer shown.
        void setLayer(scene::Layer& layer);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;
        void onTransformChanges() final;
        void onSizeChanges() final;
        void refreshDisplay() final;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Refresh the from the layer size.
        void refreshSize();

        //! Refresh the representation of the view rectangle.
        void refreshViewIndicator();

        //! @}

    private:

        // View elements
        scene::Layer* m_layer = nullptr;    //!< The layer to use for the minimap.
        sf::View m_view;                    //!< The minimap view.

        // Decorum
        sf::RectangleShape m_background;            //!< The background.
        sf::RectangleShape m_layerViewIndicator;    //!< The rectangle of the view.
    };
}
