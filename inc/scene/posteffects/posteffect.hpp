#pragma once

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace scene
{
    //! An post-processed effect to be applied to the full target.
    //! Can consist of multiple shader passes.

    class PostEffect
    {
    public:

        //! Constructor.
        PostEffect();

        //! Default destructor.
        virtual ~PostEffect() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Apply the post effect to out from the texture in.
        virtual void apply(sf::RenderTarget& out, const sf::RenderTexture& in) = 0;

        //! @}

    protected:

        //--------------//
        //! @name Tools
        //! @{

        //! Apply a shader to the whole target.
        void shaderize(sf::RenderTarget& out, const sf::Shader& shader, sf::BlendMode blendMode = sf::BlendNone);

        //! @}

    private:

        sf::VertexArray m_vertices; //!< The drawing rect.
        sf::RenderStates m_states;  //!< The states used for drawing.
    };
}
