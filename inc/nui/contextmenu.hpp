#pragma once

#include "nui/object.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    class ContextMenu : public Object
    {
        typedef Object baseClass;

    public:
        ContextMenu();
        virtual ~ContextMenu() {}

        // Virtual
        void init() override;
        void update() override;

    protected:
        // Updates
        virtual void changedMessage();

        // Params
        PARAMG(float, m_padding, padding)
        PARAMGSU(std::string, m_message, message, setMessage, changedMessage)

    private:
        sf::Text m_text;
        sf::RectangleShape m_bg; // TODO To be a sf::Sprite for looks
    };
}

