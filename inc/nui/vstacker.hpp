#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    class VStacker : public Stacker
    {
        using baseClass = Stacker;

    public:
        VStacker();
        virtual ~VStacker() {}

        // Virtual
        void update() override;

    protected:
        float getX(float childWidth, Align align);
        float getInitY();
        float getPreY(float childHeight);
        float getPostY(float childHeight);
    };
}
