#ifndef SPRITEOBJECTINFO_H
#define SPRITEOBJECTINFO_H

#include "universalobjectinterface.h"

#include "../override/imagefile.h"

#include "angleinfo.h"


namespace SpriterEngine
{

    class SpriteObjectInfo : public UniversalObjectInterface
    {
    public:
        SpriteObjectInfo();

        point getPosition() override;
        real getAngle() override;
        point getScale() override;
        real getAlpha() override;
        point getPivot() override;
        ImageFile *getImage() override;
        const sf::Color& getTiltColor() override;

        void setPosition(const point &newPosition) override;
        void setAngle(real newAngle) override;
        void setScale(const point &newScale) override;
        void setAlpha(real newAlpha) override;
        void setPivot(const point &newPivot) override;
        void setImage(ImageFile *newImageFile) override;
        void setSpin(int newSpin) override;
        void setTiltColor(const sf::Color& color) override;

        void setToBlendedLinear(UniversalObjectInterface * aObject, UniversalObjectInterface * bObject, real t, real blendRatio) override;

        void render() override;

    private:
        point position;
        AngleInfo angle;
        point scale;
        real alpha;
        point pivot;
        ImageFile *imageFile;
        sf::Color tiltColor = sf::Color::White;
    };

}

#endif // SPRITEOBJECTINFO_H
