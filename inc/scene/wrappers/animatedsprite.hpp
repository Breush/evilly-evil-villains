#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>
#include <Spriter/Engine.hpp>
#include <memory> // unique_ptr

// Forward declarations

namespace resources
{
    class AnimationHolder;
}

namespace scene
{
    /*! Animated sprite using a SCML file.
     *  Just load it and you are set.
     */

    class AnimatedSprite final : public Entity
    {
        friend class resources::AnimationHolder;
        using baseClass = Entity;

    public:

        //! Constructor, will register this animated sprite to the context.
        AnimatedSprite(bool isLerpable = false);

        //! Destructor, will unregister this animated sprite from the context.
        virtual ~AnimatedSprite();

        std::string _name() const final { return "AnimatedSprite"; }

        //------------------//
        //! @name Animation
        //! @{

        //! Load a specific SCML.
        void load(const std::string& id);

        //! Selects the animation to be played.
        void select(const std::wstring& animationName);

        //! Advance the animation of a certain time.
        void forward(const sf::Time& offset);

        //! Restart the current animation.
        void restart();

        //! @}

        //----------------------//
        //! @name Extra control
        //! @{

        //! Set if the animation should run.
        inline void setStarted(bool inStarted) { m_started = inStarted; }

        //! Is the current animation running?
        //! Also control if the animation has been stopped because not looping.
        bool started() const;

        //! Add a tilt color to all the sprite composing the animation.
        void setTiltColor(const sf::Color& color);

        //! @}

        //---------------//
        //! @name Hitbox
        //! @{

        //! The current hitbox.
        inline const sf::FloatRect& hitbox() const { return m_hitbox; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void drawInternal(sf::RenderTarget& target, sf::RenderStates states) const final;
        void updateRoutine(const sf::Time& dt) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the spriter entity transform to the current position/scale/etc.
        void refreshSpriterEntityTransform();

        //! @}

    private:

        // Animation
        SpriterEngine::EntityInstance* m_spriterEntity = nullptr;   //!< The entity itself.
        std::wstring m_currentAnimationName;                        //!< Current animation name.
        bool m_started = true;                                      //!< Whether the animation should run or not.

        // Color
        sf::Color m_tiltColor = sf::Color::White;   //!< Extra coloring of the sprites.

        // Hitbox
        sf::FloatRect m_hitbox;     //!< The current hitbox as defined in the animation file (if any).
        bool m_hasHitbox = false;   //!< Do we evaluate the hitbox?
    };
}
