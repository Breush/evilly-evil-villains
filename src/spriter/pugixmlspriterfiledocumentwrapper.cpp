#include "spriter/pugixmlspriterfiledocumentwrapper.hpp"

#include "spriter/pugixmlspriterfileelementwrapper.hpp"
#include "tools/string.hpp"

namespace SpriterEngine
{
    PugiXmlSpriterFileDocumentWrapper::PugiXmlSpriterFileDocumentWrapper()
    {
    }

    void PugiXmlSpriterFileDocumentWrapper::loadFile(std::string fileName)
    {
        doc.load_file(fileName.c_str());
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileDocumentWrapper::newElementWrapperFromFirstElement()
    {
        return new PugiXmlSpriterFileElementWrapper(doc.first_child());
    }

    SpriterFileElementWrapper * PugiXmlSpriterFileDocumentWrapper::newElementWrapperFromFirstElement(const std::string & elementName)
    {
        return new PugiXmlSpriterFileElementWrapper(doc.child(toWString(elementName).c_str()));
    }

}
