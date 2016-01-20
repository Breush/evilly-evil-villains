#pragma once

#include "nui/tooltip.hpp"
#include "scene/entity.hpp"

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
        //! The entity has to be detectable for the tooltip to work.
        void setTooltip(const std::wstring& tooltipString);

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onFocusChanged() final;
        void update(const sf::Time& dt, const float factor) final;
        void drawInternal(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        virtual bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        virtual void handleMouseLeft() override;

        //! @}

    private:

        // Focusing
        sf::RectangleShape m_focusSprite;       //!< The focus sprite displayed.
        sf::Shader* m_focusShader = nullptr;    //!< The focus shader.
        float m_focusAnimation = 0.f;           //!< Current delay of focus animation.

        // Tooltip
        nui::Tooltip m_tooltip;                             //!< The tooltip indeed.
        bool m_tooltipEnabled = false;                      //!< Whether the tooltip is not empty.
        bool m_showTooltip = false;                         //!< Whether the tooltip is currently shown.
        sf::Time m_tooltipTime;                             //!< Current time before showing tooltip.
        const sf::Time m_tooltipDelay = sf::seconds(0.9f);  //!< How much time to wait before showing tooltip.
    };
}
