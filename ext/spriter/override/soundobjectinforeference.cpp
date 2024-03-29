#include "override/soundobjectinforeference.h"

#include "file/filereference.h"

namespace SpriterEngine
{

    SoundObjectInfoReference::SoundObjectInfoReference()
    {
    }

    int SoundObjectInfoReference::getTriggerCount()
    {
        return triggerCount;
    }

    real SoundObjectInfoReference::getVolume()
    {
        return volume;
    }

    real SoundObjectInfoReference::getPanning()
    {
        return panning;
    }

    void SoundObjectInfoReference::setTriggerCount(int newTriggerCount)
    {
        triggerCount = newTriggerCount;
    }

    void SoundObjectInfoReference::setVolume(real newVolume)
    {
        volume = newVolume;
        setPlaybackVolume();
    }

    void SoundObjectInfoReference::setPanning(real newPanning)
    {
        panning = newPanning;
        setPlaybackPanning();
    }

    void SoundObjectInfoReference::setPlaybackVolume()
    {
    }

    void SoundObjectInfoReference::setPlaybackPanning()
    {
    }

}
