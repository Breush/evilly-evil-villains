template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename)
{
    // Create and load resource
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(filename))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

    // If loading successful, insert resource to map
    insertResource(id, std::move(resource));
    insertFilename(id, filename);
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
    insertFilename(id, filename);
}

template <typename Resource, typename Identifier>
inline Resource& ResourceHolder<Resource, Identifier>::get(Identifier id)
{
    auto found = m_resourcesMap.find(id);
    massert(found != m_resourcesMap.end(), "Resource not found");

    return *found->second;
}

template <typename Resource, typename Identifier>
inline const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
    auto found = m_resourcesMap.find(id);
    massert(found != m_resourcesMap.end(), "Resource not found");

    return *found->second;
}

template <typename Resource, typename Identifier>
inline Identifier ResourceHolder<Resource, Identifier>::getID(const std::string& filename)
{
    auto found = m_filenameMap.find(filename);
    massert(found != m_filenameMap.end(), "ID from filename " + filename + " not found");
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
inline void ResourceHolder<Resource, Identifier>::insertFilename(Identifier id, const std::string& filename)
{
    // Insert and check success
    auto inserted = m_filenameMap.insert(std::make_pair(filename, id));
    massert(inserted.second, "Cannot add filename " + filename + ". Was it already added?");
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
    massert(found != m_resourcesMap.end(), "Resource not found");

    found->second->setParameter(name, param);
}

