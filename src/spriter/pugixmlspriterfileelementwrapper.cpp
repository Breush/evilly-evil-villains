#include "spriter/pugixmlspriterfileelementwrapper.hpp"

#include "spriter/pugixmlspriterfileattributewrapper.hpp"
#include "tools/string.hpp"

namespace SpriterEngine
{
    PugiXmlSpriterFileElementWrapper::PugiXmlSpriterFileElementWrapper(ElementPtr_t initialElement):
        element(initialElement)
    {
    }

    std::string PugiXmlSpriterFileElementWrapper::getName()
    {
        return toString(std::wstring(element.name()));
    }

    bool PugiXmlSpriterFileElementWrapper::isValid()
    {
        return element;
    }

    void PugiXmlSpriterFileElementWrapper::advanceToNextSiblingElement()
    {
        element = element.next_sibling();
    }

    void PugiXmlSpriterFileElementWrapper::advanceToNextSiblingElementOfSameName()
    {
        element = element.next_sibling(element.name());
    }

    SpriterFileAttributeWrapper * PugiXmlSpriterFileElementWrapper::newAttributeWrapperFromFirstAttribute()
    {
        return new PugiXmlSpriterFileAttributeWrapper(element.first_attribute());
    }

    SpriterFileAttributeWrapper * PugiXmlSpriterFileElementWrapper::newAttributeWrapperFromFirstAttribute(const std::string & attributeName)
    {
        return new PugiXmlSpriterFileAttributeWrapper(element.attribute(toWString(attributeName).c_str()));
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementWrapperFromFirstElement()
    {
        return new PugiXmlSpriterFileElementWrapper(element.first_child());
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementWrapperFromFirstElement(const std::string & elementName)
    {
        return new PugiXmlSpriterFileElementWrapper(element.child(toWString(elementName).c_str()));
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementWrapperFromNextSiblingElement()
    {
        return new PugiXmlSpriterFileElementWrapper(element.next_sibling(element.name()));
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileElementWrapper::newElementClone()
    {
        return new PugiXmlSpriterFileElementWrapper(element);
    }

    PugiXmlSpriterFileElementWrapper::AttributePtr_t PugiXmlSpriterFileElementWrapper::findAttributeInElement(const std::string & name)
    {
        return element.attribute(toWString(name).c_str());
    }

}
