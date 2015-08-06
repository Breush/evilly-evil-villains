#include "dcb/gaugesmanager.hpp"

#include "core/gettext.hpp"

using namespace dcb;

GaugesManager::GaugesManager()
{
    setDetectable(false);

    // Stacking
    attachChild(m_globalStacker);
    for (uint i = 0u; i < m_stackers.size(); ++i) {
        auto& stacker = m_stackers[i];
        auto& gauge = m_gauges[i];
        auto& label = m_labels[i];

        m_globalStacker.stackBack(stacker);
        stacker.stackBack(gauge, nui::Align::CENTER);
        stacker.stackBack(label, nui::Align::CENTER);

        label.setPrestyle(sfe::Label::Prestyle::NUI);
    }

    // Gauges
    m_gauges[GaugeID::APPRECIATION].setFillColor({253u, 253u, 150u});
    m_gauges[GaugeID::CONFUSION].setFillColor({119u, 190u, 119u});
    m_gauges[GaugeID::TRUST].setFillColor({119u, 158u, 203u});
    m_gauges[GaugeID::CONVICTION].setFillColor({225u, 105u, 97u});
    m_gauges[GaugeID::CONVICTION].addIndicator(60.f, {194u, 59u, 34u});

    // Labels
    m_labels[GaugeID::APPRECIATION].setText(_("Appreciation"));
    m_labels[GaugeID::CONFUSION].setText(_("Confusion"));
    m_labels[GaugeID::TRUST].setText(_("Trust"));
    m_labels[GaugeID::CONVICTION].setText(_("Conviction"));

    refreshDisplay();
}

//-------------------//
//----- Routine -----//

void GaugesManager::onChildSizeChanges(scene::Entity&)
{
    setSize(m_globalStacker.size());
}

void GaugesManager::refreshDisplay()
{
    baseClass::refreshDisplay();
}

//--------------------------//
//----- Gauges control -----//

void GaugesManager::randomGauges(float seed, float min, float max)
{
    float mod = max - min + 1.f;
    m_gauges[GaugeID::APPRECIATION].setValue(min + std::fmod(seed, mod));   seed *= 48271.f;
    m_gauges[GaugeID::CONFUSION].setValue(min + std::fmod(seed, mod));      seed *= 48271.f;
    m_gauges[GaugeID::TRUST].setValue(min + std::fmod(seed, mod));          seed *= 48271.f;
    m_gauges[GaugeID::CONVICTION].setValue(min + std::fmod(seed, mod));
}

void GaugesManager::addToGauges(const std::array<float, GaugeID::COUNT>& gaugesValues)
{
    for (uint i = 0u; i < GaugeID::COUNT; ++i)
        m_gauges[i].addValue(gaugesValues[i]);
}
