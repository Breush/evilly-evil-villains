#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! An area with scroll-bars (if needed) that keeps its specific size.
    /*!
     *  One usally affects a stacker with unknown or variable size as content.
     */

    class ScrollArea final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        ScrollArea();

        //! Default destructor.
        ~ScrollArea() = default;

        //---------------------------//
        //! @name Content management
        //! @{

        //! The entity used as content.
        void setContent(scene::Entity& entity);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseWheelMoved(const int delta, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //!

        //! Recompute bar visibility and lengths.
        void refreshBars();

        //! Refresh clip area and position of the content.
        void refreshContentStatus();

        //! @}

    private:

        scene::Entity* m_content = nullptr; //!< The entity used as content.
        sf::Vector2f m_offset = {0.f, 0.f}; //!< The start for the visible top-left corner of the entity.

        // Bars
        sf::RectangleShape m_vBar;  //!< Vertical bar.
        sf::RectangleShape m_hBar;  //!< Horizontal bar.
        sf::Vector2f m_barsLength;  //!< Horizontal/vertical bars lengths.
        sf::Vector2f m_barsOffset;  //!< Horizontal/vertical bars offsets.

        // Bars grabbing
        bool m_vBarGrabbed = false;         //!< True if grabbing the vertical bar.
        bool m_hBarGrabbed = false;         //!< True if grabbing the horizontal bar.
        sf::Vector2f m_mouseStartGrabbing;  //!< Position of mouse when start grabbing.
        sf::Vector2f m_offsetStartGrabbing; //!< Position of content offset when start grabbing.

        // Decorum
        float m_barThick = 0.f; //!< The bar size.
    };
}

