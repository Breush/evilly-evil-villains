#pragma once

#include "scene/entity.hpp"

namespace dungeon
{
    //! Game entity that can fire callbacks on environment conditions.

    class DetectEntity : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        DetectEntity(bool isLerpable);

        //! Default destructor.
        virtual ~DetectEntity();

        //! Key that identify the type of entity.
        virtual std::string detectKey() const = 0;

    protected:

        //! Check if an entity with matching key is currently in range.
        bool isInRange(const std::string& key, const float range) const;
    };
}
