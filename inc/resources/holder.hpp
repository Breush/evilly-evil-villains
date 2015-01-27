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
    void load(Identifier id, const std::string& filename, const Parameter& param);

    // Returns resource from ID
    Resource& get(Identifier id);
    const Resource& get(Identifier id) const;

    // If a load has been done, return ID from filename
    Identifier getID(const std::string& filename);

    // For textures
    void setSmooth(Identifier id, bool smoothActive);
    void setRepeated(Identifier id, bool repeated);

    // For shaders
    template<typename Parameter>
    void setParameter(Identifier id, const std::string& name, const Parameter& param);

protected:
    void insertResource(Identifier id, std::unique_ptr<Resource> resource);
    void insertFilename(Identifier id, const std::string& filename);

private:
    std::map<Identifier, std::unique_ptr<Resource>>	m_resourcesMap;
    std::map<std::string, Identifier> m_filenameMap;
};

#include "holder.inl"
