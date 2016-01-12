#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Text.hpp>
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

        //! The frame's title.
        //! Set it to an empty string to remove title.
        void setTitle(const std::wstring& title);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void onChildDetached(scene::Entity& child) final;
        void onChildSizeChanges(scene::Entity& child) final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Updates the total size of the frame.
        void updateSize() ;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh everything.
        void refresh();

        //! Refresh frame and content positions from current size.
        void refreshFrame();

        //! Refresh the title box and text positions.
        void refreshTitle();

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

        // Title
        sf::Text m_titleText;               //!< Title text.
        sf::RectangleShape m_titleLeft;     //!< Title background left part.
        sf::RectangleShape m_titleMiddle;   //!< Title background middle part.
        sf::RectangleShape m_titleRight;    //!< Title background right part.
        float m_titleLeftWidth = 0.f;       //!< Title background left texture width.
        float m_titleRightWidth = 0.f;      //!< Title background right texture width.

        // Decorum
        float m_fontSize = 0.f; //!< Font size as defined in NUI guides.
        float m_vPadding = 0.f; //!< Vertical padding as defined in NUI guides.
        float m_hPadding = 0.f; //!< Horizontal padding as defined in NUI guides.
    };
}
