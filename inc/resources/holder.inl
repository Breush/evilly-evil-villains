
#include "tools/string.hpp" // toWString

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id)
{
    insertResource(id, std::unique_ptr<Resource>(new Resource));
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename, bool store)
{
    // Create and load resource
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(filename))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

    // If loading successful, insert resource to map
    insertResource(id, std::move(resource));
    if (store) storeID(id, toWString(filename));
}

template <typename Resource, typename Identifier>
template <typename Parameter>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename, const Parameter& secondParam)
{
    // Create and load resource
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(filename, secondParam))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

    // If loading successful, insert resource to map
    insertResource(id, std::move(resource));
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::loadVoid(Identifier id)
{
    m_resourcesMap.insert(std::make_pair(id, nullptr));
}

template <typename Resource, typename Identifier>
inline Resource& ResourceHolder<Resource, Identifier>::get(Identifier id)
{
    auto found = m_resourcesMap.find(id);

    if (found == m_resourcesMap.end()) {
        if (id != Identifier::DEFAULT) {
            std::cerr << "/!\\ Resource not found. Using DEFAULT instead." << std::endl;
            return get(Identifier::DEFAULT);
        } else {
            throw std::runtime_error("DEFAULT resource not found. Ouch.");
        }
    }

    return *found->second;
}

template <typename Resource, typename Identifier>
inline const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
    auto found = m_resourcesMap.find(id);

    if (found == m_resourcesMap.end()) {
        if (id != Identifier::DEFAULT) {
            std::cerr << "/!\\ Resource not found. Using DEFAULT instead." << std::endl;
            return get(Identifier::DEFAULT);
        } else {
            throw std::runtime_error("DEFAULT resource not found. Ouch.");
        }
    }

    return *found->second;
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::storeID(Identifier id, const std::wstring& filename)
{
    insertFilename(id, filename);
}

template <typename Resource, typename Identifier>
inline Identifier ResourceHolder<Resource, Identifier>::getID(const std::wstring& filename)
{
    auto found = m_filenameMap.find(filename);
    wassert(found != m_filenameMap.end(), L"ID from filename " << filename << L" not found");
    return found->second;
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource)
{
    // Insert and check success
    auto inserted = m_resourcesMap.insert(std::make_pair(id, std::move(resource)));
    massert(inserted.second, "Cannot add resource");
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::insertFilename(Identifier id, const std::wstring& filename)
{
    // Insert and check success
    auto inserted = m_filenameMap.insert(std::make_pair(filename, id));
    wassert(inserted.second, L"Cannot add filename " << filename << L". Was it already added?");
}

// For textures
template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::setSmooth(Identifier id, bool smoothActive)
{
    auto found = m_resourcesMap.find(id);
    massert(found != m_resourcesMap.end(), "Resource not found");

    found->second->setSmooth(smoothActive);
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::setRepeated(Identifier id, bool repeated)
{
    auto found = m_resourcesMap.find(id);
    massert(found != m_resourcesMap.end(), "Resource not found");

    found->second->setRepeated(repeated);
}

// For shaders
template <typename Resource, typename Identifier>
template <typename Parameter>
inline void ResourceHolder<Resource, Identifier>::setParameter(Identifier id, const std::string& name, const Parameter& param)
{
    auto found = m_resourcesMap.find(id);
    returnif (found == m_resourcesMap.end());

    found->second->setParameter(name, param);
}

