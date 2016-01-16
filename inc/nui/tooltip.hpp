#pragma once

#include "scene/entity.hpp"
#include "sfe/richtext.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! Basically a string in some small box.

    class Tooltip final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Tooltip();

        //! Destructor.
        ~Tooltip() = default;

        inline std::string _name() const { return "nui::Tooltip"; }

        //----------------//
        //! @name Control
        //! @{

        //! Set the text to be displayed.
        void setText(const std::wstring& text);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Update the absolute size.
        void updateSize();

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the background to fit the text size.
        void refresh();

        //! @}

    private:

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

        // Text
        sfe::RichText m_text;   //!< The text inside.

        // Decorum
        float m_vPadding = 0.f;     //!< Vertical padding as defined in NUI guides.
        float m_hPadding = 0.f;     //!< Horizontal padding as defined in NUI guides.
    };
}
