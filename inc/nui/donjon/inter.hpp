#pragma once

#include "nui/object.hpp"
#include "nui/contextmenu.hpp"
#include "drawables/grid.hpp"

namespace nui
{
    class DonjonInter : public nui::Object
    {
    public:
        DonjonInter();
        virtual ~DonjonInter() {}

        // Virtual
        void init() override;
        void update() override;
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos) override;
        bool handleKeyboardEvent(const sf::Event& event) override;
        //bool handleJoystickEvent(const sf::Event& event) override;

    protected:
        void changedSize() override;

    private:
        sfe::Grid m_grid;
        nui::ContextMenu m_contextMenu;

        sf::Vector2u m_selectedRoom;
    };
}
