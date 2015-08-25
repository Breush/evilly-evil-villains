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

namespace sfe
{
    /*! Animated sprite using a SCML file.
     *  Just load it and you are set.
     */

    class AnimatedSprite : public scene::Entity
    {
        friend class resources::AnimationHolder;
        using baseClass = scene::Entity;

    public:

        //! Constructor, will register this animated sprite to the context.
        AnimatedSprite(bool isLerpable = false);

        //! Destructor, will unregister this animated sprite from the context.
        virtual ~AnimatedSprite();

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

        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Internally called to update the animation state.
        void updateAnimation(const sf::Time& dt);

        //! Refresh all entities.
        void refresh();

        //! @}

    private:

        //! The SCML entity which composer the animation.
        std::list<std::unique_ptr<scml::Entity>> m_entities;

        //! Whether the animation is started or not.
        bool m_started;

        //! The selected sub-animation.
        int m_number;
    };
}
