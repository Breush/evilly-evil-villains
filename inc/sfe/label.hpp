#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Text.hpp>

// Forward declarations

enum class FontID : uint8;

namespace sfe
{
    /** sfe::Label
     *      Just a wrapper of sf::Text to a scene::Entity
     **/

    class Label : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        enum class Prestyle
        {
            NONE,
            NUI,
            NUI_SOBER,
            NUI_TITLE,
            MENU_TITLE,
            MENU_POPUP_TITLE,
            MENU_SOBER,
            MENU_SOBER_LIGHT,
        };

    public:
        Label();
        virtual ~Label() = default;

        void setFont(FontID fontID);
        void setColor(const sf::Color& color);
        void setCharacterSize(uint characterSize);
        void setText(const std::wstring& text);
        void setText(const std::wstring& text, FontID fontID);
        void setPrestyle(Prestyle prestyle);

    protected:
        void updateSize();
        void refreshNUI(const config::NUIGuides& cNUI) final;

    private:

        Prestyle m_prestyle = Prestyle::NONE;
        sf::Text m_text;

        // Decorum
        float m_fontSize;   //!< Reference font size.
    };
}
