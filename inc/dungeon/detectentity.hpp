#pragma once

#include "scene/entity.hpp"

#include <functional>

namespace dungeon
{
    //! Game entity that can fire callbacks on environment conditions.

    class DetectEntity : public scene::Entity
    {
        using baseClass = scene::Entity;

        using DetectCondition = std::function<bool()>;
        using DetectCallback = std::function<void()>;

    public:

        //! Constructor.
        DetectEntity(bool isLerpable);

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

        //! Analyse a condition string and create a corresponding function.
        //! If an error occurs, the function always returns false.
        DetectCondition interpretDetectCondition(const std::string& key, const std::string& condition);

        //! Check if an entity with matching key is currently in range.
        bool isInRange(const std::string& key, const float range) const;

        //! Factor applied to all range checks.
        inline void setDetectRangeFactor(const float detectRangeFactor) { m_detectRangeFactor = detectRangeFactor; }

        //! @}

        //! A signal testing a condition.
        struct DetectSignal
        {
            DetectCondition condition;
            DetectCallback callback;
        };

    private:

        //! Signals to activate regularly.
        std::vector<DetectSignal> m_detectSignals;

        float m_detectRangeFactor = 1.f;    //!< Factor applied to all range checks.
    };
}
