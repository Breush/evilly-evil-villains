#pragma once

#include "scml/interface.hpp"
#include "resources/holder.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <list>
#include <map>

// Forward declarations

namespace scene
{
    class AnimatedSprite;
}

namespace scml
{
    class Data;
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
        inline void setDefault(const std::string& id) { m_scmlHolder.setDefault(id); }

        //! @}

    protected:

        //-------------------------//
        //! @name Animated sprites
        //! @{

        //! Register an animated sprite in the list.
        void push(scene::AnimatedSprite* animatedSprite);

        //! Unregister an animated sprite.
        void pop(scene::AnimatedSprite* animatedSprite);

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Get the scml data.
        scml::Data& getData(const std::string& id);

        //! Get the scml file system.
        scml::FileSystem& getFileSystem(const std::string& id);

        //! @}

    private:

        //! The SCML files.
        SCMLHolder m_scmlHolder;

        //! Associated file systems.
        std::map<std::string, std::unique_ptr<scml::FileSystem>> m_fsMap;

        //! The animated sprites list.
        std::list<scene::AnimatedSprite*> m_animatedSprites;
    };
}
