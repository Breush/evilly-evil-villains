#include "animation/mainlinekeyinstance.h"

#include "timeinfo/timeinfo.h"
#include "objectref/objectrefinstance.h"

#include "animation/mainlinekey.h"

namespace SpriterEngine
{

    MainlineKeyInstance::MainlineKeyInstance(EntityInstance *entityInstance, EntityInstanceData *entityInstanceData, MainlineKey *mainlineKey)
    {
        mainlineKey->setupRefInstancesAndTimeInfo(entityInstance, entityInstanceData, &refs, &zOrder, timeInfo);
    }

    MainlineKeyInstance::~MainlineKeyInstance()
    {
        for (auto& it : refs)
        {
            delete it;
        }
    }

    real MainlineKeyInstance::process(real currentTime, ObjectInterfaceVector **instanceZOrder)
    {
        currentTime = timeInfo->getEasedTime(currentTime);
        for (auto& it : refs)
        {
            it->process(currentTime);
        }
        *instanceZOrder = &zOrder;
        return currentTime;
    }

    real MainlineKeyInstance::getTime()
    {
        return timeInfo->getTime();
    }

    real MainlineKeyInstance::getNextTime()
    {
        return timeInfo->getNextTime();
    }

    bool MainlineKeyInstance::timeInRange(real timeToCheck)
    {
        return timeInfo->timeInRange(timeToCheck);
    }

}
