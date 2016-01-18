#pragma once

#include "nui/entity.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>

#include <functional>

namespace nui
{
    //! An interactive selector for a number.

    class Slider final : public nui::Entity
    {
        using baseClass = nui::Entity;

        using ValueChangedCallback = std::function<void(uint)>;

    public:

        //! Constructor.
        Slider();

        //! Default destructor.
        ~Slider() final = default;

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

        //! How many indicators are shown.
        //! Set it to -1u to make it show all.
        void setVisibleSteps(uint visibleSteps);

        //! Change the color of the indicator;
        void setIndicatorTiltColor(const sf::Color& tiltColor);

        //! @}

        //------------------//
        //! @name Callbacks
        //! @{

        //! Set a callback to call each time the value changed.
        void setValueChangedCallback(ValueChangedCallback valueChangedCallback);

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

        //! Update the absolute size.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        inline bool handleGlobalEvents() const final { return true; }
        void handleGlobalMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& nuiPos) final;
        void handleGlobalMouseMoved(const sf::Vector2f& nuiPos) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos);
        void handleMouseLeft();
        bool handleKeyboardEvent(const sf::Event& event) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh all elements positions.
        void refreshElements();

        //! Refresh the bar display.
        void refreshBar();

        //! Refresh the visible traces.
        void refreshTraces();

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

        // Configuration
        uint m_visibleSteps = -1u;  //!< How many step to show.
        uint m_value = 0u;  //!< The current value.
        uint m_min = 0u;    //!< The min for the range.
        uint m_max = 0u;    //!< The max for the range.

        // Callbacks
        ValueChangedCallback m_valueChangedCallback;    //!< Called each time the value changed.

        // States
        bool m_grabbing = false;    //!< Whether we are currently grabbing the indicator.

        // Bar
        sf::RectangleShape m_barLeft;   //!< Bar left part.
        sf::RectangleShape m_barMiddle; //!< Bar middle part.
        sf::RectangleShape m_barRight;  //!< Bar right part.
        float m_barLeftWidth = 0.f;     //!< Bar left part width.
        float m_barRightWidth = 0.f;    //!< Bar right part width.

        // Traces
        std::vector<Trace> m_traces;                    //!< The traces.
        const sf::Texture* m_traceTexture = nullptr;    //!< Texture used for the traces' lines.
        float m_traceWidth = 0.f;                       //!< Trace line texture width.

        // Indicator
        sf::RectangleShape m_indicator; //!< The current value indicator.
        float m_indicatorWidth = 0.f;   //!< The indicator texture width.

        // Decorum
        float m_barOffset = 0.f;        //!< Bar vertical offset.
        float m_widthHint = 0.f;        //!< The width hint for the object.
        float m_heightHint = 0.f;       //!< The height hint for the object.
        float m_fontSize = 0.f;         //!< Font size.
    };
}
