#pragma once

#include "scene/entity.hpp"
#include "dungeon/event.hpp"
#include "nui/hstacker.hpp"
#include "nui/vstacker.hpp"
#include "sfe/gauge.hpp"
#include "sfe/label.hpp"

namespace dcb
{
    //! Controls the four gauges in the DBC negociation screen.

    class GaugesManager final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        GaugesManager();

        //! Default destructor.
        ~GaugesManager() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onChildSizeChanges(scene::Entity& child);
        void refreshDisplay() final;

        //! @}

        //! The four gauges.
        enum GaugeID
        {
            APPRECIATION,
            CONFUSION,
            TRUST,
            CONVICTION,
            COUNT,
        };

    private:

        nui::HStacker m_globalStacker;  //!< Main stacker containing other stackers.
        std::array<nui::VStacker, GaugeID::COUNT> m_stackers;   //!< The stackers, one for each pair gauge/label.
        std::array<sfe::Gauge, GaugeID::COUNT> m_gauges;        //!< The gauges.
        std::array<sfe::Label, GaugeID::COUNT> m_labels;        //!< The labels.
    };
}
