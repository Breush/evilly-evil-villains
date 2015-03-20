#pragma once

#include "nui/stacker.hpp"

namespace nui
{
    class VStacker : public Stacker
    {
        using baseClass = Stacker;

    public:
        VStacker();
        virtual ~VStacker() = default;

        // Virtual
        void update() override;
        void refreshDisplay() final;

    protected:
        float getX(float childWidth, Align align);
        float getInitY();
        float getPreY(float childHeight);
        float getPostY(float childHeight);
    };
}
