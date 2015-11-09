#include "spriter/pugixmlspriterfileattributewrapper.hpp"

#include "tools/string.hpp"

namespace SpriterEngine
{
    PugiXmlSpriterFileAttributeWrapper::PugiXmlSpriterFileAttributeWrapper(AttributePtr_t initialAttribute):
        attribute(initialAttribute)
    {
    }

    std::string PugiXmlSpriterFileAttributeWrapper::getName()
    {
        return toString(std::wstring(attribute.name()));
    }

    bool PugiXmlSpriterFileAttributeWrapper::isValid()
    {
        return attribute;
    }

    real PugiXmlSpriterFileAttributeWrapper::getRealValue()
    {
        return attribute.as_double();
    }

    int PugiXmlSpriterFileAttributeWrapper::getIntValue()
    {
        return attribute.as_int();
    }

    std::string PugiXmlSpriterFileAttributeWrapper::getStringValue()
    {
        return toString(std::wstring(attribute.as_string()));
    }

    void PugiXmlSpriterFileAttributeWrapper::advanceToNextAttribute()
    {
        attribute = attribute.next_attribute();
    }
}
