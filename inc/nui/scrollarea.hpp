#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/sprite.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! An area with scroll-bars (if needed) that keeps its specific size.
    /*!
     *  One usually affects a stacker with unknown or variable size as content.
     */

    class ScrollArea final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        ScrollArea();

        //! Default destructor.
        ~ScrollArea() = default;

        std::string _name() const final { return "nui::ScrollArea"; }

        //---------------------------//
        //! @name Content management
        //! @{

        //! The entity used as content.
        void setContent(scene::Entity& entity);

        //! @}

        //----------------------------//
        //! @name Getters and setters
        //! @{

        //! Get the bar thickness.
        inline float barThick() const { return m_barMiddleThick; }

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

        inline bool handleGlobalEvents() const final { return true; }
        void handleGlobalMouseMoved(const sf::Vector2f& nuiPos) final;
        void handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseWheelMoved(const int delta, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //!

        //! Recompute the offset for the nuiPos of the bar grabbed.
        //! The grabber will be set to the projected nuiPos.
        void refreshOffsetFromPosition(const sf::Vector2f& nuiPos);

        //! Recompute bar visibility and lengths.
        void refreshBars();

        //! Refresh clip area and position of the content.
        void refreshContentStatus();

        //! @}

    private:

        scene::Entity* m_content = nullptr;         //!< The entity used as content.
        sf::Vector2f m_offset = {0.f, 0.f};         //!< The start for the visible top-left corner of the entity.
        sf::Vector2f m_availableSize = {0.f, 0.f};  //!< The visible size of the content.

        // Bars
        sf::Sprite m_vBarStart;             //!< The vertical bar start.
        sf::Sprite m_hBarStart;             //!< The horizontal bar start.
        sf::RectangleShape m_vBarMiddle;    //!< The vertical bar middle.
        sf::RectangleShape m_hBarMiddle;    //!< The horizontal bar middle.
        sf::Sprite m_vBarEnd;               //!< The vertical bar end.
        sf::Sprite m_hBarEnd;               //!< The horizontal bar end.
        float m_barMiddleThick = 0.f;       //!< Bar thickness.
        float m_barMiddleStep = 0.f;        //!< Top and bottom textures padding for the middle bar.

        // Bars grabbing
        scene::Sprite m_vGrabber;       //!< The displayed grabber for the vertical bar.
        scene::Sprite m_hGrabber;       //!< The displayed grabber for the horizontal bar.
        bool m_vBarGrabbed = false;     //!< True if grabbing the vertical bar.
        bool m_hBarGrabbed = false;     //!< True if grabbing the horizontal bar.
        sf::Vector2f m_vGrabberSize;    //!< The grabber texture size.
    };
}

