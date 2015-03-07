#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    class TextEntry : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        TextEntry();
        virtual ~TextEntry() = default;

        // Routine
        void update();

    protected:
        // Events
        void handleKeyboardEvent(const sf::Event& event) override;

    private:
        // Background
        sf::RectangleShape m_background;

        // Text
        sf::String m_string;
        sf::Text m_text;
    };
}
