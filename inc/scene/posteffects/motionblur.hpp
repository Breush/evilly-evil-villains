#pragma once

#include "scene/posteffects/posteffect.hpp"

namespace scene
{
    //! A custom cel-shading post-effect.

    class MotionBlur final : public PostEffect
    {
    public:

        //! Constructor.
        MotionBlur();

        //! Default destructor.
        ~MotionBlur() = default;

        //----------------//
        //! @name Routine
        //! @{

        void apply(sf::RenderTarget& out, const sf::RenderTexture& in) final;

        //! @}

    protected:

        //---------------//
        //! @name Effect
        //! @{

        //! Resize and create all the textures if necessary.
        void prepareTextures(const sf::Vector2u& size);

        //! Applying the additive shader.
        void mix(sf::RenderTarget& out, const sf::RenderTexture& in1, const sf::RenderTexture& in2);

        //! @}

    private:

        sf::Shader* m_mixShader = nullptr;  //!< The mixing shader.

        sf::RenderTexture m_memoryTexture;  //!< The memory texture.
        sf::RenderTexture m_drawingTexture; //!< The drawing texture.
    };
}
