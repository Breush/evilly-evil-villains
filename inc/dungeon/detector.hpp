#pragma once

#include "tools/int.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Time.hpp>

#include <functional>
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

        using UID_t = uint32;
        using DetectionLambda = std::function<void(DetectEntity& entity, float distance)>;

    public:

        // Default constructor.
        Detector() = default;

        // Default destructor.
        ~Detector() = default;

        //------------------//
        //! @name Detection
        //! @{

        //! Find an entity with the corresponding UID.
        DetectEntity* find(const UID_t UID);

        //! Find an entity with the corresponding UID.
        const DetectEntity* find(const UID_t UID) const;

        //! Apply a lambda for each entity in the range.
        void applyInRange(const sf::Vector2f& position, float range, DetectionLambda rangeEntityFunc);

        //! @}

    protected:

        //------------------//
        //! @name Detection
        //! @{

        //! Check if there is an entity with matching key in range of the one specified.
        //! @return The UID of the entity detected, or -1u if none.
        std::vector<UID_t> inRangeUIDs(const DetectEntity& entity, const std::string& key, const float range) const;

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

        //! Remember the next UID to be set.
        UID_t m_UIDGenerator = 0u;
    };

    //! Global detector.
    extern Detector s_detector;
}
