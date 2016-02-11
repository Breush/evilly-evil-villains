#pragma once

#include "scene/posteffects/posteffect.hpp"

namespace scene
{
    //! A custom cel-shading post-effect.

    class Floomzig final : public PostEffect
    {
    public:

        //! Constructor.
        Floomzig();

        //! Default destructor.
        ~Floomzig() = default;

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

        //! Applying the sobel shader.
        void sobel(sf::RenderTarget& out, const sf::RenderTexture& in);

        //! Applying the additive shader.
        void add(sf::RenderTarget& out, const sf::RenderTexture& in1, const sf::RenderTexture& in2);

        //! @}

    private:

        sf::Shader* m_sobelShader = nullptr;    //!< The floomzig shader.
        sf::Shader* m_addShader = nullptr;      //!< The additive shader.

        sf::RenderTexture	m_sobelTexture;     //!< The floomzig texture.
    };
}
