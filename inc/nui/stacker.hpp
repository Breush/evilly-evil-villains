#pragma once

#include "scene/entity.hpp"

#include <list>

namespace nui
{
    class Stacker : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        // Defines children position
        enum class Align {
            STANDARD,
            CENTER,
            OPPOSITE,
        };

    public:
        Stacker();
        virtual ~Stacker() = default;

        // Children management
        void add(scene::Entity* child, Align align = Align::STANDARD);
        void clear();

    protected:
        // Pure virtual
        virtual void update() override = 0;

        // Params
        PARAMGSU(float, m_margin, margin, setMargin, update)
        PARAMGSU(Align, m_align, align, setAlign, update)

        struct ChildInfo {
            scene::Entity* entity;
            Align align;
        };

    protected:
        // Children
        std::list<ChildInfo> m_children;
    };
}

