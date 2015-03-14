#pragma once

#include "tools/debug.hpp"
#include "tools/tools.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <string>
#include <memory>
#include <stdexcept>

namespace resources
{
    template <typename Resource, typename Identifier>
    class ResourceHolder
    {
    public:
        void load(Identifier id, const std::string& filename, bool store = false);

        template <typename Parameter>
        void load(Identifier id, const std::string& filename, const Parameter& param);

        void load(Identifier id);       //!< Will create a not loaded unique_ptr<Resource>
        void loadVoid(Identifier id);   //!< Will associate nullptr to the identifier

        // Returns resource from ID
        Resource& get(Identifier id);
        const Resource& get(Identifier id) const;

        // If a load has been done, return ID from filename
        void storeID(Identifier id, const std::wstring& filename);
        Identifier getID(const std::wstring& filename);

        // For textures
        void setSmooth(Identifier id, bool smoothActive);
        void setRepeated(Identifier id, bool repeated);

        // For shaders
        template<typename Parameter>
        void setParameter(Identifier id, const std::string& name, const Parameter& param);

    protected:
        void insertResource(Identifier id, std::unique_ptr<Resource> resource);
        void insertFilename(Identifier id, const std::wstring& filename);

    private:
        std::map<Identifier, std::unique_ptr<Resource>>	m_resourcesMap;
        std::map<std::wstring, Identifier> m_filenameMap;
    };
}

//----- Few type definitions

// Forward declarations

enum class TextureID : uint8;
enum class ShaderID : uint8;
enum class FontID : uint8;
enum class SoundID : uint8;
enum class AnimationID : uint8;

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
    using TextureHolder =       ResourceHolder<sf::Texture, TextureID>;
    using ShaderHolder =        ResourceHolder<sf::Shader, ShaderID>;
    using FontHolder =          ResourceHolder<sf::Font, FontID>;
    using SoundBufferHolder =   ResourceHolder<sf::SoundBuffer, SoundID>;
    using SCMLHolder =          ResourceHolder<scml::Data, AnimationID>;
}

#include "holder.inl"
