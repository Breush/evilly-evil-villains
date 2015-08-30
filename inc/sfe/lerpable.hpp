#pragma once

#include "tools/param.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

// Forward declarations

namespace scene
{
    class Entity;
}

namespace sfe
{
    /*! This component of an Entity allows to interpolate various parameters over time.
     *  For example, set a targetPosition and a speed, the entity will move at that speed
     *  to the provided targetPosition.
     */

    class Lerpable
    {
        friend class scene::Entity;

    public:

        //! Constructor.
        Lerpable(scene::Entity* entity);

        //! Default destructor.
        virtual ~Lerpable() = default;

        //-----------------------//
        //! @name User interface
        //! @{

        //! Need to be called to set current variables as default reference for offset functions.
        void saveDefaults();

        //! @}

        //-------------------------------//
        //! @name Position interpolation
        //! @{

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

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether position interpolation is activated or not.
        PARAMGS(bool, m_positionLerping, positionLerping, setPositionLerping)

        //! The speed for position interpolation.
        PARAMGS(sf::Vector2f, m_positionSpeed, positionSpeed, setPositionSpeed)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        //! Update the various parameters of the entity.
        void update(const sf::Time& dt);

        //! @}

        //----------------------//
        //! @name Interpolation
        //! @{

        //! Find next position given the original one and the stored target.
        sf::Vector2f nextPosition(sf::Vector2f position, const sf::Time& dt);

        //! Low-level procedure to make converge source into target if the correct direction.
        void converge(float& source, const float target, const float offset);

        //! @}

    private:

        scene::Entity* m_entity = nullptr;  //!< The binded entity.

        sf::Vector2f m_targetPosition;      //!< The target position.
        sf::Vector2f m_defaultPosition;     //!< The default position, for offset relative.
    };
}

