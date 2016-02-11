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

        using UID_t = uint32;
        using DetectCondition = std::function<std::vector<UID_t>()>;
        using DetectCallback = std::function<void(const UID_t)>;

    public:

        //! Constructor.
        DetectEntity();

        //! Default destructor.
        virtual ~DetectEntity();

        //! Key that identify the type of entity.
        virtual std::string detectKey() const = 0;

        //------------------//
        //! @name Detection
        //! @{

        //! The UID of the entity.
        inline UID_t UID() const { return m_UID; }

        //! Whether this entity is to be considered in signal checks.
        inline bool detectVisible() const { return m_detectVisible; }

        //! Whether this entity does signal checks.
        inline bool detectActive() const { return m_detectActive; }

        //! Set whether this entity is to be considered in range checks.
        inline void setDetectVisible(bool detectVisible) { m_detectVisible = detectVisible; }

        //! Set whether this entity does range checks.
        inline void setDetectActive(bool detectActive) { m_detectActive = detectActive; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update(const sf::Time &dt, const float factor) final;

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
        std::vector<UID_t> inRangeUIDs(const std::string& key, const float range) const;

        //! Factor applied to all range checks.
        inline void setDetectRangeFactor(const float detectRangeFactor) { m_detectRangeFactor = detectRangeFactor; }

        //! The factor applied to all range checks.
        inline float detectRangeFactor() const { return m_detectRangeFactor; }

        //! Set the UID of the entity, should be called by detector.
        inline void setUID(UID_t inUID) { m_UID = inUID; }

        //! @}

        //! A signal testing a condition.
        struct DetectSignal
        {
            DetectCondition condition;
            DetectCallback callback;
        };

    private:

        //! The UID of the entity, will be defined by the detector.
        UID_t m_UID = -1u;

        //! Signals to activate regularly.
        std::vector<DetectSignal> m_detectSignals;

        float m_detectRangeFactor = 1.f;    //!< Factor applied to all range checks.
        bool m_detectVisible = true;        //!< Is this entity to be considered in signal checks?
        bool m_detectActive = true;         //!< Is this entity doing signal checks?
    };
}
