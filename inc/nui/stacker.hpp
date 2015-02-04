#pragma once

#include "nui/object.hpp"

#include <list>

namespace nui
{
    class Stacker : public Object
    {
        typedef Object baseClass;

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

        // Virtual
        void init() override {}
        virtual void update() override = 0;
        void changedChild(Object* child) override;

        // Children management
        void add(Object* child);
        void add(Object* child, Align align);

    protected:
        // Changes
        void changedStatus() override;

        // Params
        PARAMGSU(float, m_margin, margin, setMargin, update)
        PARAMGSU(Align, m_align, align, setAlign, update)

        struct ChildInfo {
            Object* child;
            Align align;
        };

    protected:
        // Children
        std::list<ChildInfo> m_children;
    };
}

