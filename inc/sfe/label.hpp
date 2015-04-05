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
        enum class PrestyleID {
            MENU_TITLE,
            MENU_POPUP_TITLE,
            MENU_SOBER,
        };

    public:
        Label();
        virtual ~Label() = default;

        void setFont(FontID fontID);
        void setCharacterSize(uint characterSize);
        void setText(const std::wstring& text);
        void setText(const std::wstring& text, FontID fontID);
        void setPrestyle(PrestyleID prestyle);

    protected:
        void updateSize();
        void refreshDisplay() final;

    private:
        sf::Text m_text;
    };
}
