#pragma once

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Time.hpp>

#include <string>
#include <vector>
#include <list>

namespace dungeon
{
    // Forward declarations

    class DetectEntity;

    //! Detect proximity of entities and inform them consequently.

    class Detector final : private sf::NonCopyable
    {
        friend class DetectEntity;

    public:

        // Default constructor.
        Detector() = default;

        // Default destructor.
        ~Detector() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Regular update.
        void update(const sf::Time& dt);

        //! @}

    protected:

        //------------------//
        //! @name Detection
        //! @{

        //! Check if there is an entity with matching key in range of the one specified.
        bool isInRange(const DetectEntity& entity, const std::string& key, const float range) const;

        //! @}

        //--------------------//
        //! @name Registering
        //! @{

        //! Register the entity.
        //! Should be called by the constructor of DetectEntity.
        void addEntity(DetectEntity& entity);

        //! Remove the entity.
        //! Should be called by the destructor of DetectEntity.
        void removeEntity(DetectEntity& entity);

        //! @}

        //! Type of action for pending ones.
        enum class Action
        {
            ADD,
            REMOVE,
        };

        //! Actions we delayed.
        struct PendingAction
        {
            Action action;
            DetectEntity* pEntity;
        };

    private:

        //! All registered entities.
        std::list<DetectEntity*> m_entities;

        //! Actions we delayed.
        std::vector<PendingAction> m_pendingActions;
    };

    //! Global detector.
    extern Detector s_detector;
}
