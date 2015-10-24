#pragma once

#include "nui/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    //! An interactive selector for a number.

    class RangeSelector final : public nui::Entity
    {
        using baseClass = nui::Entity;

    public:

        //! Constructor.
        RangeSelector();

        //! Default destructor.
        ~RangeSelector() final = default;

        std::string _name() const final { return "nui::RangeSelector"; }

        //----------------------//
        //! @name Configuration
        //! @{

        //! Set the limits for min/max values.
        void setRange(uint min, float max);

        //! Set the current value.
        void setValue(uint inValue);

        //! Get the current value.
        inline uint value() const { return m_value; }

        //! Set the value to the closest one near the relative x position.
        void setClosestValue(const float relX);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! An hint for the length of the bar (default 100).
        PARAMGSU(float, m_length, length, setLength, updateSize)
        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Set the size of the bar, given the length and nui configuration.
        void updateSize();

        //! Refresh all elements positions.
        void refreshElements();

        //! Refresh the current value indicator.
        void refreshIndicator();

        //! @}

        //! Traces.
        struct Trace
        {
            sf::RectangleShape line;    //!< The vertical line.
            sf::Text text;              //!< Yhe text.
        };

    private:

        uint m_value = 0u;  //!< The current value.
        uint m_min = 0u;    //!< The min for the range.
        uint m_max = 0u;    //!< The max for the range.

        bool m_grabbing = false;    //!< Whether we are currently grabbing the indicator.

        // Decorum
        std::vector<Trace> m_traces;    //!< The traces.
        sf::RectangleShape m_mainLine;  //!< The main line.
        sf::RectangleShape m_indicator; //!< The current value indicator.
        float m_widthHint = 0.f;        //!< The width hint for the object.
        float m_heightHint = 0.f;       //!< The height hint for the object.
        float m_fontSize = 0.f;         //!< Font size.
    };
}
