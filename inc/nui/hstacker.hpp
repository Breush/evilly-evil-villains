#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    class HStacker : public Stacker
    {
        using baseClass = Stacker;

    public:
        HStacker();
        virtual ~HStacker() = default;

        // Virtual
        void update() override;
        void refreshDisplay() final;

    protected:
        float getY(float childHeight, Align align);
        float getInitX();
        float getPreX(float childWidth);
        float getPostX(float childWidth);
    };
}
