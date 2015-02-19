#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Text.hpp>

namespace sfe
{
    /** sfe::Label
     *      Just a wrapper of sf::Text to a scene::Entity
     **/

    class Label : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        Label();
        virtual ~Label() {}

        void setFont(Fonts::ID fontID);
        void setCharacterSize(uint characterSize);
        void setText(const std::wstring& text);

    protected:
        void updateSize();

    private:
        sf::Text m_text;
    };
}
