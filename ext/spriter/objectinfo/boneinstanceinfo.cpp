#include "objectinfo/boneinstanceinfo.h"

namespace SpriterEngine
{

    BoneInstanceInfo::BoneInstanceInfo(point initialSize) :
        scale(1, 1),
        alpha(1),
        size(initialSize)
    {
    }

    point BoneInstanceInfo::getPosition()
    {
        return position;
    }

    real BoneInstanceInfo::getAngle()
    {
        return angle.angle;
    }

    point BoneInstanceInfo::getScale()
    {
        return scale;
    }

    real BoneInstanceInfo::getAlpha()
    {
        return alpha;
    }

    point BoneInstanceInfo::getSize()
    {
        return size;
    }


    void BoneInstanceInfo::setPosition(const point &newPosition)
    {
        position = newPosition;
    }

    void BoneInstanceInfo::setAngle(real newAngle)
    {
        angle.angle = newAngle;
    }

    void BoneInstanceInfo::setScale(const point &newScale)
    {
        scale = newScale;
    }

    void BoneInstanceInfo::setAlpha(real newAlpha)
    {
        alpha = newAlpha;
    }

    void BoneInstanceInfo::setToBlendedLinear(UniversalObjectInterface *aObject, UniversalObjectInterface *bObject, real t, real blendRatio)
    {
        real tempAngle = angle.angle;
        point tempPosition = position;
        point tempScale = scale;
        real tempAlpha = alpha;

        aObject->setObjectToLinear(bObject, t, this);

        setAngle(shortestAngleLinear(tempAngle, angle.angle, blendRatio));
        setPosition(linear(tempPosition, position, blendRatio));
        setScale(linear(tempScale, scale, blendRatio));
        setAlpha(linear(tempAlpha, alpha, blendRatio));
    }
}
