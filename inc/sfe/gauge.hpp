#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace sfe
{
    //! A variable gauge entity to show a percentage.

    class Gauge final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Gauge();

        //! Default destructor.
        ~Gauge() final = default;

        //----------------------//
        //! @name Configuration
        //! @{

        //! Set the color of the filler.
        void setFillColor(const sf::Color& color);

        //! Set the limits for min/max values.
        void setLimits(float limitMin, float limitMax);

        //! Set the value, which will be truncated if not between the min/max limits.
        void setValue(float inValue);

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Get the current value, as a proportion of min/max limits.
        float value();

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! An hint for the length of the bar (default 100).
        PARAMGSU(float, m_length, length, setLength, updateSize)

        //! Whether the gauge is filled vertically or horizontally.
        PARAMGSU(bool, m_verticalOrientation, verticalOrientation, setVerticalOrientation, refreshFiller)

        //! Whether the gauge is filled left -> right (bottom -> top) or the opposite way.
        PARAMGSU(bool, m_invertedAppearance, invertedAppearance, setInvertedAppearance, refreshFiller)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update() final;
        void refreshDisplay() final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{
        
        //! Set the size of the bar, given the length and nui configuration.
        void updateSize();

        //! Updates the visual part of the filler.
        void refreshFiller();

        //! @}

    private:

        sf::RectangleShape m_background;    //!< The background.
        sf::RectangleShape m_filler;        //!< The displayed percentage.

        float m_percent = 0.42f;            //!< How much of the gauge should be filled. Always between 0 and 1.
        float m_minLimit = 0.f;             //!< The low limit so that percent equals 0.
        float m_maxLimit = 100.f;           //!< The high limit so that percent equals 1.
        
        float m_minHintSize;    //!< NUI proportional minimum side size.
        float m_maxHintSize;    //!< NUI proportional maximum side size.
    };
}
