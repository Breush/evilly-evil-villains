template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::load(Identifier id, const std::string& filename)
{
    // Create and load resource
    std::unique_ptr<Resource> resource(new Resource());
    if (!resource->loadFromFile(filename))
        throw std::runtime_error("ResourceHolder::load - Failed to load " + filename);

    // If loading successful, insert resource to map
    insertResource(id, std::move(resource));
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
inline Resource& ResourceHolder<Resource, Identifier>::get(Identifier id)
{
    auto found = mResourceMap.find(id);
    massert(found != mResourceMap.end(), "Resource not found");

    return *found->second;
}

template <typename Resource, typename Identifier>
inline const Resource& ResourceHolder<Resource, Identifier>::get(Identifier id) const
{
    auto found = mResourceMap.find(id);
    massert(found != mResourceMap.end(), "Resource not found");

    return *found->second;
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::insertResource(Identifier id, std::unique_ptr<Resource> resource)
{
    // Insert and check success
    auto inserted = mResourceMap.insert(std::make_pair(id, std::move(resource)));
    massert(inserted.second, "Cannot add resource");
}

// For textures
template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::setSmooth(Identifier id, bool smoothActive)
{
    auto found = mResourceMap.find(id);
    massert(found != mResourceMap.end(), "Resource not found");

    found->second->setSmooth(smoothActive);
}

template <typename Resource, typename Identifier>
inline void ResourceHolder<Resource, Identifier>::setRepeated(Identifier id, bool repeated)
{
    auto found = mResourceMap.find(id);
    massert(found != mResourceMap.end(), "Resource not found");

    found->second->setRepeated(repeated);
}

// For shaders
template <typename Resource, typename Identifier>
template <typename Parameter>
inline void ResourceHolder<Resource, Identifier>::setParameter(Identifier id, const std::string& name, const Parameter& param)
{
    auto found = mResourceMap.find(id);
    massert(found != mResourceMap.end(), "Resource not found");

    found->second->setParameter(name, param);
}

