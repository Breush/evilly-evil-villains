#pragma once

#include "scene/entity.hpp"
#include "context/event.hpp"
#include "dungeon/sidebar/rankingbar.hpp"
#include "dungeon/sidebar/resourcebar.hpp"
#include "nui/vstacker.hpp"
#include "nui/frame.hpp"

#include <memory>
#include <array>

namespace dungeon
{
    // Forward declarations

    class Data;

    //! A small window with main dungeon information.

    class Summary final : public scene::Entity, private context::EventReceiver
    {
        using baseClass = scene::Entity;

        //! The resources bars.
        enum ResourceBarID : uint8
        {
            RESOURCE_TIME,
            RESOURCE_DOSH,
            RESOURCE_SOUL,
            RESOURCE_COUNT, //!< Keep last.
        };

        //! The ranking bars.
        enum RankingBarID : uint8
        {
            RANKING_RANK,   //!< Based on villain's evilness.
            RANKING_CLASS,  //!< Based on dungeon's fame.
            RANKING_COUNT,  //!< Keep last.
        };

    public:

        //! Constructor.
        Summary();

        //! Default destructor.
        ~Summary() = default;

        std::string _name() const final { return "dungeon::Summary"; }

        //! Initialize after textures are loaded.
        void init();

        //---------------------//
        //! @name Dungeon data
        //! @{

        //! The data of the dungeon to be read from.
        void useData(Data& data);

        //! @}

        //----------------//
        //! @name Control
        //! @{

        //! Set the width used.
        void setWidth(float width);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) final;

        //! Set the perfect size for a correct display.
        void updateSize();

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the decorum to the newly set size.
        void refresh();

        //! To be called whenever the dungeon data changed.
        void refreshFromData();

        //! To be called whenever time changed.
        void refreshTimeBar();

        //! To be called whenever dosh changed.
        void refreshDoshBar();

        //! To be called whenever soul changed.
        void refreshSoulBar();

        //! To be called whenever evil changed.
        void refreshRankBar();

        //! To be called whenever fame changed.
        void refreshClassBar();

        //! @}

    private:

        Data* m_data;   //!< The data of the dungeon to be read from.

        // Content
        nui::Frame m_frame;                                         //!< The box around.
        nui::VStacker m_stacker;                                    //!< The bars' stacker.
        std::array<RankingBar, RANKING_COUNT> m_rankingBars;        //!< The bars specifing ranking.
        std::array<ResourceBar, RESOURCE_COUNT> m_resourcesBars;    //!< The bars specifing resources.

        // Control
        float m_width = 0.f;    //!< The width of the entity, as defined by the user.
    };
}
