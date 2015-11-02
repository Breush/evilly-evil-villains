#pragma once

#include "scene/entity.hpp"
#include "scene/grabbable.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "context/event.hpp"
#include "ai/dumbpuppet.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! A MonsterGrabbable spawner.
    /*!
     *  Implements the scene::GrabbableSpawner for a monster reserve.
     *  The player grabs and moves the monster into the dungeon.
     */

    class MonsterCage final : public scene::Entity, public scene::GrabbableSpawner, public context::EventReceiver
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor, affecting texture and look.
        MonsterCage(std::wstring monsterID, Inter& inter, Data& data);

        //! Default destructor.
        ~MonsterCage() = default;

        std::string _name() const final { return "dungeon::MonsterCage"; }

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;
        void refreshNUI(const config::NUIGuides& cNUI) override;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        void receive(const context::Event& event) final;
        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //------------------//
        //! @name Grabbable
        //! @{

        void grabbableMoved(Entity *entity, const sf::Vector2f &relPos, const sf::Vector2f &nuiPos) final;
        void grabbableButtonReleased(Entity* entity, const sf::Mouse::Button button, const sf::Vector2f& relPos, const sf::Vector2f& nuiPos) final;
        std::unique_ptr<scene::Grabbable> spawnGrabbable() final;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the cost labels colors given the current dosh held by the player.
        void refreshCostLabelsColor();

        //! Refresh the number of puppets in the reserve.
        void refreshReserve();

        //! @}

    private:

        Inter& m_inter;             //!< Reference to the dungeon inter.
        Data& m_data;               //!< Reference to the whole data.
        std::wstring m_monsterID;   //!< The monster to be managed in this cage.

        // Reserve
        std::vector<std::unique_ptr<ai::DumbPuppet>> m_puppets; //!< Puppet that moves from left to right.

        // Decorum
        sf::RectangleShape m_background;    //!< Background.
        scene::RichLabel m_baseCostLabel;   //!< Text for base cost.
        float m_scaleFactor = 1.f;          //!< Scaling for puppets.
    };

    //! A monster temporary object.
    /*!
     *  Implements the scene::Grabbable for the a dungeon monster.
     */

    class MonsterGrabbable final : public scene::Grabbable
    {
        using baseClass = scene::Grabbable;

    public:

        //! Constructor, affecting texture and look.
        MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID);

        //! Default destructor.
        ~MonsterGrabbable() = default;

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

    private:

        //! Holding the image of the grabbable.
        scene::AnimatedSprite m_sprite;
    };
}
