#ifndef SOUNDOBJECTINFOREFERENCE_H
#define SOUNDOBJECTINFOREFERENCE_H

#include "../objectinfo/universalobjectinterface.h"

namespace SpriterEngine
{

    class FileReference;

    class SoundObjectInfoReference : public UniversalObjectInterface
    {
    public:
        SoundObjectInfoReference();

        int getTriggerCount() override;

        real getVolume() override;
        real getPanning() override;

        void setTriggerCount(int newTriggerCount) override;

        void setVolume(real newVolume) override;
        void setPanning(real newPanning) override;

        // TODO: override playTrigger() to play back sound using getTriggerCount() to determine whether to play;

    private:
        virtual void setPlaybackVolume();
        virtual void setPlaybackPanning();

        real volume = 1.;
        real panning = 0.;

        int triggerCount = 0;
    };

}

#endif // SOUNDOBJECTINFOREFERENCE_H
