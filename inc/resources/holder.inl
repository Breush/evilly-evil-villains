#include "tools/platform-fixes.hpp" // make_unique

#include <stdexcept> // runtime_error

namespace resources
{
    //-------------------//
    //----- Storage -----//

    template <typename Resource>
    inline Resource& ResourceHolder<Resource>::load(const std::string& filename)
    {
        // Create and load resource
        auto resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(filename))
            throw std::runtime_error("[ResourceHolder] Failed to load '" + filename + "'. Ouch.");

        // If loading successful, insert resource to map
        return insertResource(getID(filename), std::move(resource));
    }

    template <typename Resource>
    template <typename Parameter>
    inline Resource& ResourceHolder<Resource>::load(const std::string& filename, const Parameter& parameter)
    {
        // Create and load resource
        auto resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(filename, parameter))
            throw std::runtime_error("[ResourceHolder] Failed to load '" + filename + "'. Ouch.");

        // If loading successful, insert resource to map
        return insertResource(getID(filename), std::move(resource));
    }

    template <typename Resource>
    inline void ResourceHolder<Resource>::setDefault(const std::string& id)
    {
        auto found = m_resourcesMap.find(id);
        if (found == m_resourcesMap.end())
            throw std::runtime_error("[ResourceHolder] Resource '" + id + "' not found. Cannot set it as default backup.");

        m_default = found->second.get();
    }

    //------------------//
    //----- Access -----//

    template <typename Resource>
    inline std::string ResourceHolder<Resource>::getID(const std::string& filename) const
    {
        // Remove res/xxx/ and file extension
        std::string id = filename.substr(0u, filename.find_last_of("."));
        id = id.substr(id.find_first_of("/") + 1u);
        return id.substr(id.find_first_of("/") + 1u);
    }

    template <typename Resource>
    inline bool ResourceHolder<Resource>::stored(const std::string& id) const
    {
        auto found = m_resourcesMap.find(id);
        return (found != m_resourcesMap.end());
    }

    template <typename Resource>
    inline Resource& ResourceHolder<Resource>::get(const std::string& id)
    {
        auto found = m_resourcesMap.find(id);
        if (found == m_resourcesMap.end()) {
            if (m_default == nullptr)
                throw std::runtime_error("[ResourceHolder] Resource '" + id + "' is not a valid ID. No default backup. Ouch.");
            return *m_default;
        }

        return *found->second;
    }

    template <typename Resource>
    inline const Resource& ResourceHolder<Resource>::get(const std::string& id) const
    {
        auto found = m_resourcesMap.find(id);
        if (found == m_resourcesMap.end()) {
            if (m_default == nullptr)
                throw std::runtime_error("[ResourceHolder] Resource '" + id + "' is not a valid ID. No default backup. Ouch.");
            return *m_default;
        }

        return *found->second;
    }

    template <typename Resource>
    inline Resource& ResourceHolder<Resource>::insertResource(std::string id, std::unique_ptr<Resource> resource)
    {
        auto inserted = m_resourcesMap.insert(std::make_pair(std::move(id), std::move(resource)));
        if (!inserted.second)
            throw std::runtime_error("[ResourceHolder] Unable to insert resource '" + id + "'. Ouch.");

        return *inserted.first->second;
    }
}

