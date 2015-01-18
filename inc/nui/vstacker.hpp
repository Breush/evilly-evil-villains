#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    #if 0
    class VStacker : public Stacker
    {
        typedef Stacker baseClass;

    public:
        VStacker(Object* parent, uiCore* core);
        virtual ~VStacker() {}

    protected:
        uint getX(uint& childWidth, Align& align);
        uint getInitY();
        int getPreY(uint& childHeight);
        int getPostY(uint& childHeight);

        // Virtual
        void recomputeChildrenPos() override;
    };
    #endif
}
