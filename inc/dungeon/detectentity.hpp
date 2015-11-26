#pragma once

#include "scene/entity.hpp"

#include <functional>

namespace dungeon
{
    // Forward declarations

    class Detector;

    //! Game entity that can fire callbacks on environment conditions.

    class DetectEntity : public scene::Entity
    {
        using baseClass = scene::Entity;

        friend class Detector;

        using DetectCondition = std::function<uint32()>;
        using DetectCallback = std::function<void(const uint32)>;

    public:

        //! Constructor.
        DetectEntity(bool isLerpable = false);

        //! Default destructor.
        virtual ~DetectEntity();

        //! Key that identify the type of entity.
        virtual std::string detectKey() const = 0;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update(const sf::Time &dt) final;

        //! @}

        //------------------//
        //! @name Detection
        //! @{

        //! Add a function to call whenever a condition is met concerning an entity with the specified key.
        //! For exemple, key = "hero", condition = "distance < 50".
        void addDetectSignal(const std::string& key, const std::string& condition, DetectCallback callback);

        //! Clear all detect signals from the list.
        void removeDetectSignals();

        //! Analyse a condition string and create a corresponding function.
        //! If an error occurs, the function always returns false.
        DetectCondition interpretDetectCondition(const std::string& key, const std::string& condition);

        //! Check if an entity with matching key is currently in range.
        uint32 isInRange(const std::string& key, const float range) const;

        //! Factor applied to all range checks.
        inline void setDetectRangeFactor(const float detectRangeFactor) { m_detectRangeFactor = detectRangeFactor; }

        //! The UID of the entity.
        inline uint32 UID() const { return m_UID; }

        //! Set the UID of the entity, should be called by detector.
        inline void setUID(uint32 inUID) { m_UID = inUID; }

        //! @}

        //! A signal testing a condition.
        struct DetectSignal
        {
            DetectCondition condition;
            DetectCallback callback;
        };

    private:

        //! The UID of the entity, will be defined by the detector.
        uint32 m_UID = -1u;

        //! Signals to activate regularly.
        std::vector<DetectSignal> m_detectSignals;

        float m_detectRangeFactor = 1.f;    //!< Factor applied to all range checks.
    };
}
