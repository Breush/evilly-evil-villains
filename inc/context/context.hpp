#pragma once

#include "context/commander.hpp"
#include "resources/holder.hpp"
#include "resources/musicplayer.hpp"
#include "resources/soundplayer.hpp"
#include "resources/animationholder.hpp"
#include "config/windowinfo.hpp"
#include "config/nuiguides.hpp"
#include "config/display.hpp"
#include "config/audio.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Font.hpp>

namespace context
{
    //! All resources managers, window information and configurations.
    //! Basically all global application-wide things.

    class Context final : sf::NonCopyable
    {
    public:

        //! Constructor.
        Context() : windowInfo(display.window) {}

        //! Recreates the window from current config::WindowInfo.
        void recreateWindow();

        sf::RenderWindow window;        //!< The window, final destination of all draws.
        context::Commander commander;   //!< The commands manager.

        resources::TextureHolder    textures;   //!< The textures.
        resources::ShaderHolder     shaders;    //!< The shaders.
        resources::FontHolder       fonts;      //!< The fonts.
        resources::MusicPlayer      musics;     //!< The musics.
        resources::SoundPlayer      sounds;     //!< The sounds.
        resources::AnimationHolder  animations; //!< The animations, parsed SCML files.

        config::Display     display;    //!< The display configuration.
        config::Audio       audio;      //!< The audio configuration.
        config::NUIGuides   nuiGuides;  //!< Guidelines for NUI elements.
        config::WindowInfo  windowInfo; //!< Extra informations for window parameters.
    };

    //! Access the context.
    extern Context context;
}
