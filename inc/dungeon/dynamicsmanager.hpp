#pragma once

#include "context/event.hpp"
#include "dungeon/elements/dynamic.hpp"

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! A hero invading the dungeon.

    class DynamicsManager final : public context::EventReceiver
    {
    public:

        //! Constructor.
        DynamicsManager();

        //! Default destructor.
        ~DynamicsManager() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Update routine.
        void update(const sf::Time& dt);

        //! @}

        //-------------------------//
        //! @name File interaction
        //! @{

        //! Load from an XML file.
        void load(const pugi::xml_node& node);

        //! Save to an XML file.
        void save(pugi::xml_node node);

        //! @}

        //----------------------------//
        //! @name Dungeon interaction
        //! @{

        //! Set the dungeon inter source.
        void useInter(Inter& inter);

        //! @}

        //-------------------------//
        //! @name Dynamics control
        //! @{

        //! Create a new dynamic element at the specified position.
        uint32 create(const sf::Vector2f& rpos, const std::wstring& id);

        //! Remove the dynamic.
        void remove(const Dynamic* pDynamic);

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh all element data reference for dynamics.
        void refreshDynamicsData();

        //! @}

    private:

        //! Basic dynamic status, modifications occurs during update.
        enum class DynamicStatus
        {
            RUNNING,        //!< Dynamic is ok, standard status.
            TO_BE_REMOVED,  //!< Dynamic will be removed.
        };

        //! Contains an Dynamic plus its status.
        struct DynamicInfo
        {
            std::unique_ptr<Dynamic> dynamic = nullptr; //!< Dynamic pointer.
            DynamicStatus status;                       //!< Status.
            ElementData data;                           //!< All its data.
        };

    private:

        // Shared data
        Data* m_data = nullptr;     //!< Dungeon data.
        Inter* m_inter = nullptr;   //!< The dungeon inter, to get cellsize and position.

        // Dynamics
        std::vector<DynamicInfo> m_dynamicsInfo;    //!< All the dynamics currently in the dungeon.
    };
}
