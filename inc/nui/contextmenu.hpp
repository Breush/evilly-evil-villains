#pragma once

#include "nui/object.hpp"

#include <SFML/Graphics/Text.hpp>

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
        // Params

    private:
        sf::Text m_text;
    };
}

