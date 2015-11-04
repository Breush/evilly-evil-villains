#pragma once

#include "resources/holder.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <Spriter/Engine.hpp>
#include <list>
#include <map>

// Forward declarations

namespace scene
{
    class AnimatedSprite;
}

namespace resources
{
    //! Holder for animations.
    /*!
     *  This class has two purposes:
     *  - Manage a list of active animated sprites, in order to update them ;
     *  - Combine SCMLHolder to a map of file systems.
     */
    class AnimationHolder final : private sf::NonCopyable
    {
        friend class scene::AnimatedSprite;

    public:

        //! Default constructor.
        AnimationHolder() = default;

        //! Default destructor.
        ~AnimationHolder() = default;

        //----------------//
        //! @name Storage
        //! @{

        //! Load animation resource into memory.
        void load(const std::string& filename);

        //! Remove an animation from memory.
        void free(const std::string& id);

        //! Remove all animations from memory with id starting with specified prefix.
        void freeMatchingPrefix(const std::string& prefix);

        //! Sets the backup resource to use when id does not exists.
        //! Provided id should exists.
        // TODO
        void setDefault(const std::string& id) {}

        //! @}

    protected:

    private:
    };
}
