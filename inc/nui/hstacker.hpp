#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    class HStacker : public Stacker
    {
        typedef Stacker baseClass;

    public:
        HStacker();
        virtual ~HStacker() {}

        // Virtual
        void update() override;

    protected:
        float getY(float& childHeight, Align& align);
        float getInitX();
        float getPreX(float& childWidth);
        float getPostX(float& childWidth);
    };
}
