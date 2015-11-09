#include "spriter/examplefilefactory.hpp"

#include <Spriter/override/imagefile.h>
#include <Spriter/override/soundfile.h>

#include "spriter/pugixmlspriterfiledocumentwrapper.hpp"
#include "spriter/sfmlimagefile.hpp"
#include "spriter/sfmlsoundfile.hpp"

namespace SpriterEngine
{

    ExampleFileFactory::ExampleFileFactory(sf::RenderWindow *validRenderWindow):
        renderWindow(validRenderWindow)
    {
    }

    ImageFile * ExampleFileFactory::newImageFile(const std::string &initialFilePath, point initialDefaultPivot)
    {
        return new SfmlImageFile(initialFilePath, initialDefaultPivot, renderWindow);
    }

    SoundFile * ExampleFileFactory::newSoundFile(const std::string & initialFilePath)
    {
        return new SfmlSoundFile(initialFilePath);
    }

    SpriterFileDocumentWrapper * ExampleFileFactory::newScmlDocumentWrapper()
    {
        return new PugiXmlSpriterFileDocumentWrapper();
    }
}
