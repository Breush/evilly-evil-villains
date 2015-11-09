#pragma once

#include <pugixml/pugixml.hpp>

#include <Spriter/override/spriterfileattributewrapper.h>

namespace SpriterEngine
{
    class PugiXmlSpriterFileAttributeWrapper : public SpriterFileAttributeWrapper
    {
        typedef pugi::xml_attribute AttributePtr_t;

    public:
        PugiXmlSpriterFileAttributeWrapper(AttributePtr_t initialAttribute);

        std::string getName() override;

        bool isValid() override;

        real getRealValue() override;
        int getIntValue() override;
        std::string getStringValue() override;

        void advanceToNextAttribute() override;

    private:
        AttributePtr_t attribute;
    };

}
