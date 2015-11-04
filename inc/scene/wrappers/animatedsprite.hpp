#pragma once

#include "scene/entity.hpp"
#include "tools/param.hpp"
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

        //! Start or stop the current animation.
        inline void setStarted(bool iStarted) { m_started = iStarted; }

        //! Whether the animation is started (running).
        inline bool started() const { return m_started; }

        //! @}

        //----------------------//
        //! @name Extra control
        //! @{

        //! Add a tilt color to all the sprite composing the animation.
        void setTiltColor(const sf::Color& color);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the animation should loop or just freeze at the end.
        PARAMGSU(bool, m_looping, looping, setLooping, refresh)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void drawInternal(sf::RenderTarget& target, sf::RenderStates states) const final;
        void updateRoutine(const sf::Time& dt) final;

        //! Refresh all entities.
        void refresh();

        //! @}

    private:

        //! The SCML entity of the animation.
        // std::unique_ptr<SpriterEngine::EntityInstance> m_entity;

        bool m_started;                             //!< Whether the animation is started or not.
        int m_number;                               //!< The selected sub-animation.
        sf::Color m_tiltColor = sf::Color::White;   //!< Extra coloring of the sprites.
    };
}
