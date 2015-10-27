#pragma once

#include "scene/entity.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "nui/gauge.hpp"
#include "scene/wrappers/label.hpp"

#include <array>

namespace dcb
{
    // Forward declarations

    class Controller;

    //! Controls the four gauges in the DBC negociation screen.

    class GaugesManager final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! The four gauges.
        enum GaugeID
        {
            APPRECIATION,
            CONFUSION,
            TRUST,
            CONVICTION,
            COUNT,
        };

    public:

        //! Constructor.
        GaugesManager();

        //! Default destructor.
        ~GaugesManager() = default;

        std::string _name() const final { return "dcb::GaugesManager"; }

        //-----------------------//
        //! @name Gauges control
        //! @{

        //! Get the value of a specific gauge.
        inline float gaugeValue(const GaugeID gaugeID) const { return m_gauges[gaugeID].value(); }

        //! Randomize the gauges from the seed.
        void randomGauges(float seed, float min, float max);

        //! Add values to all gauges.
        void addToGauges(const std::array<float, GaugeID::COUNT>& gaugesValues);

        //! Whether or not the conviction condition is met.
        bool enoughConviction() const { return m_gauges.at(CONVICTION).value() >= m_convictionIndicator; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child);

        //! @}

    private:

        float m_convictionIndicator;                            //!< The value to consider conviction has been reached.
        nui::HStacker m_globalStacker;                          //!< Main stacker containing other stackers.
        std::array<nui::VStacker, GaugeID::COUNT> m_stackers;   //!< The stackers, one for each pair gauge/label.
        std::array<nui::Gauge, GaugeID::COUNT> m_gauges;        //!< The gauges.
        std::array<scene::Label, GaugeID::COUNT> m_labels;      //!< The labels.
    };
}
