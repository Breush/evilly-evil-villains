#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <memory>

namespace scene
{
    // Forward declarations

    class Grabbable;

    /** scene::GrabbableSpawner
     *      Base for the spawners of grabbable objects
     **/

    class GrabbableSpawner
    {
    public:
        GrabbableSpawner() = default;
        virtual ~GrabbableSpawner() = default;

        virtual std::unique_ptr<Grabbable> spawnGrabbable() = 0;

    protected:
        // Params
    };

    /** scene::Grabbable
     *      Base for the spawnable grabbable object
     **/

    class Grabbable : public sf::Drawable, public sf::Transformable
    {
    public:
        Grabbable(GrabbableSpawner& spawner) : m_spawner(spawner) {}
        virtual ~Grabbable() = default;

    private:
        GrabbableSpawner& m_spawner;
    };
}
