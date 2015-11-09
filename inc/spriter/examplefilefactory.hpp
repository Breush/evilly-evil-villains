#pragma once

#include <Spriter/override/filefactory.h>

#include <SFML/Graphics/RenderWindow.hpp>

namespace SpriterEngine
{
    class ExampleFileFactory : public FileFactory
    {
    public:
        ExampleFileFactory(sf::RenderWindow *validRenderWindow);

        ImageFile *newImageFile(const std::string &initialFilePath, point initialDefaultPivot) override;

        SoundFile *newSoundFile(const std::string &initialFilePath) override;

        SpriterFileDocumentWrapper *newScmlDocumentWrapper() override;

    private:
        sf::RenderWindow *renderWindow;
    };

}
