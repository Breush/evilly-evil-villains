#pragma once

#include "scene/entity.hpp"

#include <list>

namespace nui
{
    class Stacker : public scene::Entity
    {
        typedef scene::Entity baseClass;

    public:
        // Defines children position
        enum Align {
            STANDARD,
            CENTER,
            OPPOSITE,
        };

    public:
        Stacker();
        virtual ~Stacker() {}

        // Children management
        void add(scene::Entity* child, Align align = Align::STANDARD);

    protected:
        // Pure virtual
        virtual void update() override = 0;

        // Params
        PARAMGSU(float, m_margin, margin, setMargin, update)
        PARAMGSU(Align, m_align, align, setAlign, update)

        struct ChildInfo {
            scene::Entity* child;
            Align align;
        };

    protected:
        // Children
        std::list<ChildInfo> m_children;
    };
}

