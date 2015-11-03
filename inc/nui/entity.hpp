#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/richlabel.hpp"

#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    //! Base for every NUI object.

    class Entity : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Constructor.
        Entity(bool isLerpable = false);

        //! Destructor.
        virtual ~Entity() = default;

        //! Entity unique qualifier, mostly for debugging.
        virtual std::string _name() const = 0;

        //----------------//
        //! @name Tooltip
        //! @{

        //! Set the tooltip.
        //! Have to be detectable for the tooltip to work.
        void setTooltip(std::wstring tooltipString);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void update(const sf::Time& dt) final;
        virtual void refreshNUI(const config::NUIGuides& cNUI) override;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        virtual bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        virtual void handleMouseLeft() override;

        //! @}

        //--------------------------------//
        //! @name Internal change updates
        //! @{

        //! Refresh the tooltip background.
        void refreshTooltipBackground();

        //! @}

    private:

        // Tooltip
        scene::RichLabel m_tooltipText;                     //!< The tooltip label.
        scene::RectangleShape m_tooltipBackground;          //!< The tooltip background.
        bool m_tooltipEnabled = false;                      //!< Whether the tooltip is not empty.
        bool m_showTooltip = false;                         //!< Whether the tooltip is currently shown.
        sf::Time m_tooltipTime;                             //!< Current time before showing tooltip.
        const sf::Time m_tooltipDelay = sf::seconds(0.9f);  //!< How much time to wait before showing tooltip.
    };
}
