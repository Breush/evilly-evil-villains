#pragma once

#include "scene/entity.hpp"
#include "scml/interface.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/System/Time.hpp>
#include <memory> // unique_ptr
#include <list>

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

        //! Internally called to update the animation state.
        void updateAnimation(const sf::Time& dt);

        //! Refresh all entities.
        void refresh();

        //! @}

    private:

        //! The SCML entity which composer the animation.
        std::list<std::unique_ptr<scml::Entity>> m_entities;


        bool m_started;                             //!< Whether the animation is started or not.
        int m_number;                               //!< The selected sub-animation.
        sf::Color m_tiltColor = sf::Color::White;   //!< Extra coloring of the sprites.
    };
}
