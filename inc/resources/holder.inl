#include "tools/platform-fixes.hpp" // make_unique

#include <stdexcept> // runtime_error

namespace resources
{
    //-------------------//
    //----- Storage -----//

    template <typename Resource>
    inline Resource& Holder<Resource>::load(const std::string& filename)
    {
        // Create and load resource
        auto resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(filename))
            throw std::runtime_error("[resources::Holder] Failed to load '" + filename + "'. Ouch.");

        // If loading successful, insert resource to map
        return insertResource(getID(filename), std::move(resource));
    }

    template <typename Resource>
    template <typename Parameter>
    inline Resource& Holder<Resource>::load(const std::string& filename, const Parameter& parameter)
    {
        // Create and load resource
        auto resource = std::make_unique<Resource>();
        if (!resource->loadFromFile(filename, parameter))
            throw std::runtime_error("[resources::Holder] Failed to load '" + filename + "'. Ouch.");

        // If loading successful, insert resource to map
        return insertResource(getID(filename), std::move(resource));
    }

    template <typename Resource>
    inline void Holder<Resource>::free(const std::string& id)
    {
        m_resourcesMap.erase(id);
    }

    template <typename Resource>
    inline void Holder<Resource>::freeMatchingPrefix(const std::string& prefix)
    {
        // Find matching IDs
        std::vector<std::string> matchingIDs;
        for (const auto& resource : m_resourcesMap)
            if (std::mismatch(std::begin(prefix), std::end(prefix), std::begin(resource.first)).first == std::end(prefix))
                matchingIDs.emplace_back(resource.first);

        // Remove them
        for (const auto& id : matchingIDs)
            free(id);
    }

    template <typename Resource>
    inline void Holder<Resource>::setDefault(const std::string& id)
    {
        auto found = m_resourcesMap.find(id);
        if (found == m_resourcesMap.end())
            throw std::runtime_error("[resources::Holder] Resource '" + id + "' not found. Cannot set it as default backup.");

        m_default = found->second.get();
    }

    //------------------//
    //----- Access -----//

    template <typename Resource>
    inline std::string Holder<Resource>::getID(const std::string& filename) const
    {
        // Remove res/ and file extension
        std::string id = filename.substr(0u, filename.find_last_of("."));
        return id.substr(id.find_first_of("/") + 1u);
    }

    template <typename Resource>
    inline bool Holder<Resource>::stored(const std::string& id) const
    {
        auto found = m_resourcesMap.find(id);
        return (found != m_resourcesMap.end());
    }

    template <typename Resource>
    inline Resource& Holder<Resource>::get(const std::string& id)
    {
        auto found = m_resourcesMap.find(id);
        if (found == m_resourcesMap.end()) {
            if (m_default == nullptr)
                throw std::runtime_error("[resources::Holder] Resource '" + id + "' is not a valid ID. No default backup. Ouch.");
            return *m_default;
        }

        return *found->second;
    }

    template <typename Resource>
    inline const Resource& Holder<Resource>::get(const std::string& id) const
    {
        auto found = m_resourcesMap.find(id);
        if (found == m_resourcesMap.end()) {
            if (m_default == nullptr)
                throw std::runtime_error("[resources::Holder] Resource '" + id + "' is not a valid ID. No default backup. Ouch.");
            return *m_default;
        }

        return *found->second;
    }

    template <typename Resource>
    inline Resource& Holder<Resource>::insertResource(std::string id, std::unique_ptr<Resource> resource)
    {
        auto inserted = m_resourcesMap.insert(std::make_pair(std::move(id), std::move(resource)));
        if (!inserted.second)
            throw std::runtime_error("[resources::Holder] Unable to insert resource '" + id + "'. Ouch.");

        return *inserted.first->second;
    }
}

