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
        void setLayer(const scene::Layer& layer);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;
        void onSizeChanges() final;
        void refreshDisplay() final;

        //! @}

    private:

        // View elements
        const scene::Layer* m_layer = nullptr;  //!< The layer to use for the minimap.
        sf::View m_view;                        //!< The minimap view.

        // Decorum
        sf::RectangleShape m_background;    //!< The background.
    };
}
