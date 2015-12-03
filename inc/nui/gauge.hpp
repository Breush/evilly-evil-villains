#pragma once

#include "nui/entity.hpp"
#include "sfe/line.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! A variable gauge entity to show a percentage.

    class Gauge final : public nui::Entity
    {
        using baseClass = nui::Entity;

    public:

        //! Constructor.
        Gauge();

        //! Default destructor.
        ~Gauge() final = default;

        std::string _name() const final { return "nui::Gauge"; }

        //----------------------//
        //! @name Configuration
        //! @{

        //! Set the color of the filler.
        void setFillColor(const sf::Color& color);

        //! Set the limits for min/max values.
        void setLimits(float limitMin, float limitMax);

        //! Adds a bar at a specific value.
        /*!
         *  The color used is the color of the bar, and the new fill color
         *  when the indicator is passed.
         */
        void addIndicator(float inValue, const sf::Color& color);

        //! @}

        //----------------------------//
        //! @name Getters and setters
        //! @{

        //! Sets if the gauge is vertical or not. True for vertical.
        void setVerticalOrientation(bool inVerticalOrientation);

        //! Set the value, which will be truncated if not between the min/max limits.
        void setValue(float inValue);

        //! Add to the value.
        inline void addValue(float inValue) { setValue(value() + inValue); }

        //! Substract from the value.
        inline void subValue(float inValue) { setValue(value() - inValue); }

        //! Get the current value, as a proportion of min/max limits.
        float value() const;

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! An hint for the length of the bar (default 100).
        PARAMGSU(float, m_length, length, setLength, updateSize)

        //! Whether the gauge is filled vertically or horizontally.
        PARAMG(bool, m_verticalOrientation, verticalOrientation)

        //! Whether the gauge is filled left -> right (bottom -> top) or the opposite way.
        PARAMGSU(bool, m_invertedAppearance, invertedAppearance, setInvertedAppearance, refreshFiller)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Set the size of the bar, given the length and nui configuration.
        void updateSize();

        //! Called whenever orientation changed.
        void refreshOrientation();

        //! Updates the visual part of the filler.
        void refreshFiller();

        //! Refresh the indicators positions.
        void refreshIndicators();

        //! @}

    private:

        //! Indicator line.
        struct Indicator
        {
            std::unique_ptr<sfe::HLine> hline = nullptr;    //!< The horizontal line indicator (if vertical orientation).
            std::unique_ptr<sfe::VLine> vline = nullptr;    //!< The vertical line indicator (if horizontal orientation).
            sf::Color color = sf::Color::White;             //!< The color of the indicator.
            float value = 0.f;                              //!< The value (position) of indicator.
        };

    private:

        sf::RectangleShape m_background;    //!< The background.
        sf::RectangleShape m_filler;        //!< The displayed percentage.

        float m_percent = 0.42f;            //!< How much of the gauge should be filled. Always between 0 and 1.
        float m_minLimit = 0.f;             //!< The low limit so that percent equals 0.
        float m_maxLimit = 100.f;           //!< The high limit so that percent equals 1.

        float m_minHintSize;    //!< NUI proportional minimum side size.
        float m_maxHintSize;    //!< NUI proportional maximum side size.

        std::vector<Indicator> m_indicators;    //!< All the indicators.
    };
}
