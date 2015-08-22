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
        Resource& load(const std::string& filename, const Parameter& parameter);

        //! Sets the backup resource to use when id does not exists.
        //! Provided id should exists.
        void setDefault(const std::string& id);

        //! @}

        //---------------//
        //! @name Access
        //! @{

        //! Gets the ID of the file from filename.
        //! res/xxx/dir/file.yyy -> dir/file
        std::string getID(const std::string& filename) const;

        //! Returns true if resource exists.
        bool stored(const std::string& id) const;

        //! Returns true if resource exists.
        inline bool fileStored(const std::string& filename) const { return stored(getID(filename)); };

        //! Get resource from its ID.
        Resource& get(const std::string& id);

        //! Get resource from its ID.
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

        std::map<std::string, std::unique_ptr<Resource>> m_resourcesMap;    //!< Storage.
        Resource* m_default = nullptr;  //!< Backup when an other is not found.
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
