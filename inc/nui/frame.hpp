#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! A decorative frame that goes around an other entity.

    class Frame final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Frame();

        //! Default destructor.
        ~Frame() = default;

        std::string _name() const final { return "nui::Frame"; }

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

        void onChildDetached(scene::Entity& child) final;
        void onSizeChanges() final;
        void onChildSizeChanges(scene::Entity& child) final;

        //! Updates the total size of the frame.
        void updateSize() ;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh frame and content positions from current size.
        void refreshFrame();

        //! @}

    private:

        scene::Entity* m_content = nullptr;         //!< The entity used as content.

        // Frame
        sf::Sprite m_topLeft;       //!< Top-left frame corner.
        sf::Sprite m_topRight;      //!< Top-right frame corner.
        sf::Sprite m_bottomLeft;    //!< Bottom-left frame corner.
        sf::Sprite m_bottomRight;   //!< Bottom-right frame corner.

        sf::RectangleShape m_top;       //!< Top frame border.
        sf::RectangleShape m_left;      //!< Left frame border.
        sf::RectangleShape m_right;     //!< Right frame border.
        sf::RectangleShape m_bottom;    //!< Bottom frame border.

        sf::RectangleShape m_fill;  //!< The background.

        sf::Vector2f m_cornerSize;  //!< Top-left corner size.
    };
}
