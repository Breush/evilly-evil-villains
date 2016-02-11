#pragma once

#include "scene/posteffects/posteffect.hpp"

#include <array>

namespace scene
{
    //! A bloom post-effect.

    class Bloom final : public PostEffect
    {
        using RenderTextureArray = std::array<sf::RenderTexture, 2u>;

    public:

        //! Constructor.
        Bloom();

        //! Default destructor.
        ~Bloom() = default;

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

        //! Applying the bright detection shader.
        void filterBright(sf::RenderTexture& out, const sf::RenderTexture& in);

        //! Applying the blur shader (multipass).
        void blurMultipass(RenderTextureArray& textures);

        //! Applying the blur shader.
        void blur(sf::RenderTexture& out, const sf::RenderTexture& in, const sf::Vector2f& offsetFactor);

        //! Applying the downsampling shader.
        void downsample(sf::RenderTexture& out, const sf::RenderTexture& in);

        //! Applying the additive shader.
        void add(sf::RenderTarget& out, const sf::RenderTexture& in1, const sf::RenderTexture& in2);

        //! @}

    private:

        sf::Shader* m_brightShader = nullptr;       //!< The bright shader.
        sf::Shader* m_downSampleShader = nullptr;   //!< The downsampling shader.
        sf::Shader* m_gaussianBlurShader = nullptr; //!< The gaussian blur shader.
        sf::Shader* m_addShader = nullptr;          //!< The additive shader.

        sf::RenderTexture	m_brightTexture;        //!< The bright texture.
        RenderTextureArray	m_firstPassTextures;    //!< The first pass textures.
        RenderTextureArray	m_secondPassTextures;   //!< The second pass textures.
    };
}
