#pragma once

#include <map>
#include <string>
#include <memory>

namespace resources
{
    //! Dynamic structure that keep a ressource type into memory.
    //! Used for loading once then storing textures/sounds/shaders/fonts.
    // TODO Add "setDefault(std::string)"

    template <typename Resource>
    class ResourceHolder
    {
    public:

        //----------------//
        //! @name Storage
        //! @{

        //! Load resource into memory.
        Resource& load(const std::string& filename);

        //! Load resource into memory with special parameter to loadFromFile (for shaders).
        template <typename Parameter>
        Resource& load(const std::string& id, const std::string& filename, const Parameter& parameter);

        //! @}

        //---------------//
        //! @name Access
        //! @{

        //! Returns true if resource exists.
        bool exists(const std::string& id);

        //! Get resource from its ID/file name.
        Resource& get(const std::string& id);

        //! Get resource from its ID/file name.
        const Resource& get(const std::string& id) const;

        //! @}

    protected:

        //----------------//
        //! @name Storage
        //! @{

        //! Stores resource into memory.
        Resource& insertResource(std::string id, std::unique_ptr<Resource> resource);

        //! @}

    private:

        //! Storage.
        std::map<std::string, std::unique_ptr<Resource>> m_resourcesMap;
    };
}

//----- Few type definitions

// Forward declarations

namespace sf
{
    class Texture;
    class Shader;
    class Font;
    class SoundBuffer;
    class View;
}

namespace scml
{
    class Data;
}

namespace resources
{
    using TextureHolder =       ResourceHolder<sf::Texture>;
    using ShaderHolder =        ResourceHolder<sf::Shader>;
    using FontHolder =          ResourceHolder<sf::Font>;
    using SoundBufferHolder =   ResourceHolder<sf::SoundBuffer>;
    using SCMLHolder =          ResourceHolder<scml::Data>;
}

#include "holder.inl"
