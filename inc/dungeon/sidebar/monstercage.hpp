#pragma once

#include "scene/entity.hpp"
#include "scene/grabbable.hpp"
#include "scene/wrappers/label.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "dungeon/sidebar/costbanner.hpp"
#include "context/event.hpp"
#include "ai/dumbpuppet.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace dungeon
{
    // Forward declarations

    class Data;
    class Inter;

    //! Monster panel part.
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
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //------------------//
        //! @name Grabbable
        //! @{

        //! Tries to hire a new monster.
        void hire();

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

        //! Refresh the hire box text, position and size.
        void refreshHireBox();

        //! Refresh the color of the hire box.
        void refreshHireBoxVisualLock();

        //! Refresh the number of puppets in the reserve.
        void refreshReservePuppetsCount();

        //! Refresh the parameters of the puppets in the reserve.
        void refreshReservePuppetsParameters(const uint monstersUpdateStart = 0u);

        //! @}

    private:

        Inter& m_inter;             //!< Reference to the dungeon inter.
        Data& m_data;               //!< Reference to the whole data.
        std::wstring m_monsterID;   //!< The monster to be managed in this cage.

        sf::Vector2f m_grabbablePosition;   //!< Retains the last mouse position knowned.

        // Reserve
        sf::RectangleShape m_reserveBackground;                 //!< Background for the reserve part.
        std::vector<std::unique_ptr<ai::DumbPuppet>> m_puppets; //!< Puppet that moves from left to right.
        float m_scaleFactor = 1.f;                              //!< Scaling for puppets.

        // Box
        scene::Label m_nameLabel;                   //!< Text for the monster's name.
        CostBanner m_baseCostBanner;                //!< Banner for base cost.
        CostBanner m_weeklyCostBanner;              //!< Banner for weekly cost.
        scene::RichLabel m_hireBoxLabel;            //!< Text for hire time.
        scene::RectangleShape m_hireBoxBackground;  //!< The hire time background.

        bool m_hireBoxLocked = false;   //!< Is the box countdown still counting?
        bool m_hireBoxSelected = false; //!< Is the box hovered?

        // Decorum
        float m_padding = 0.f;  //!< The padding.
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
        MonsterGrabbable(scene::GrabbableSpawner& spawner, const std::wstring& monsterID, const sf::Vector2f& scale);

        //! Default destructor.
        ~MonsterGrabbable() = default;

        std::string _name() const final { return "dungeon::MonsterGrabbable"; }

        //! Called whenever the grabbable moved.
        void movedOffset(const sf::Vector2f& dir);

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt) final;

        //! @}

    private:

        //! Holding the image of the grabbable.
        scene::AnimatedSprite m_sprite;
    };
}
