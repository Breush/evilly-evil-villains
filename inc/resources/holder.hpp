#pragma once

#include "tools/debug.hpp"

#include <SFML/System/Time.hpp>

#include <map>
#include <string>
#include <memory>
#include <stdexcept>

template <typename Resource, typename Identifier>
class ResourceHolder
{
public:
    void load(Identifier id, const std::string& filename);

    template <typename Parameter>
    void load(Identifier id, const std::string& filename, const Parameter& secondParam);

    Resource& get(Identifier id);
    const Resource& get(Identifier id) const;

    // For textures
    void setSmooth(Identifier id, bool smoothActive);
    void setRepeated(Identifier id, bool repeated);

    // For shaders
    template<typename Parameter>
    void setParameter(Identifier id, const std::string& name, const Parameter& param);

private:
    void insertResource(Identifier id, std::unique_ptr<Resource> resource);


private:
    std::map<Identifier, std::unique_ptr<Resource>>	mResourceMap;
};

#include "holder.inl"
