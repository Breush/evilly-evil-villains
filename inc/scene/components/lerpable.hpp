#pragma once

#include "scene/components/component.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

namespace scene
{
    //! Allows to interpolate various parameters over time.
    /*!
     *  For instance, set a targetPosition and a speed, the entity will move at that speed
     *  to the provided targetPosition.
     */

    class Lerpable final : public Component
    {
        using baseClass = Component;

    public:

        //! Constructor.
        Lerpable(Entity& entity);

        //! Default destructor.
        ~Lerpable() = default;

        static std::string id() noexcept { return "Lerpable"; }

        //----------------//
        //! @name Routine
        //! @{

        void update(const sf::Time& dt);

        //! @}

        //----------------------//
        //! @name Basic control
        //! @{

        //! Need to be called to set current variables as default reference for offset functions.
        void saveDefaults();

        //! @}

        //-------------------------------//
        //! @name Position interpolation
        //! @{

        //! Stop or un-pause the position lerping.
        inline void setPositionLerping(bool positionLerping) { m_positionLerping = positionLerping; }

        //! True while the position is being lerped.
        inline bool positionLerping() const { return m_positionLerping; }

        //! The speed for position lerping.
        inline void setPositionSpeed(const sf::Vector2f& positionSpeed) { m_positionSpeed = positionSpeed; }

        //! The target position.
        inline const sf::Vector2f& targetPosition() const { return m_targetPosition; }

        //! Set the target position and starts lerping.
        void setTargetPosition(const sf::Vector2f& targetPosition);

        /*! Sets a relative target position.
         *  One should have called saveDefaults() before,
         *  as the offset is considered relatively to defaultPosition.
         */
        void setTargetPositionOffset(const sf::Vector2f& positionOffset);

        //! @}

    protected:

        //----------------------//
        //! @name Interpolation
        //! @{

        //! Find next position given the original one and the stored target.
        sf::Vector2f nextPosition(sf::Vector2f position, const sf::Time& dt);

        //! Low-level procedure to make converge source into target if the correct direction.
        void converge(float& source, const float target, const float offset);

        //! @}

    private:

        // Position
        bool m_positionLerping = false;     //!< Should we lerp the position?
        sf::Vector2f m_positionSpeed;       //!< The speed to move position when lerping.
        sf::Vector2f m_targetPosition;      //!< The target position.
        sf::Vector2f m_defaultPosition;     //!< The default position, for offset relative.
    };
}

