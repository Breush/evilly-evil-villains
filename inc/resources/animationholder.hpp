#pragma once

#include "resources/holder.hpp"
#include "spriter/spriterfilefactory.hpp"
#include "spriter/spriterobjectfactory.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>
#include <Spriter/Engine.hpp>

#include <unordered_map>
#include <list>

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
        AnimationHolder();

        //! Default destructor.
        ~AnimationHolder() = default;

        //----------------//
        //! @name Storage
        //! @{

        //! Returns the id from the filename.
        std::string getID(const std::string& filename);

        //! Load animation resource into memory.
        void load(const std::string& filename);

        //! Remove an animation from memory.
        void free(const std::string& id);

        //! Remove all animations from memory with id starting with specified prefix.
        void freeMatchingPrefix(const std::string& prefix);

        //! Sets the backup resource to use when id does not exists.
        //! Provided id should exists.
        void setDefault(const std::string& id) { m_defaultID = id; }

        //! @}

    protected:

        //----------------------------//
        //! @name Spriter interfacing
        //! @{

        //! Get the model from the id.
        SpriterEngine::SpriterModel& getModel(const std::string& id);

        //! @}

    private:

        using SpriterModelPtr = std::unique_ptr<SpriterEngine::SpriterModel>;
        std::unordered_map<std::string, SpriterModelPtr> m_models;  //!< All the models.

        std::string m_defaultID;    //!< The backup solution when an animation model is not found.
    };
}
