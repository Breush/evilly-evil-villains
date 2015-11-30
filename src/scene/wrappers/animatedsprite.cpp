#include "scene/wrappers/animatedsprite.hpp"

#include "core/application.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/string.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"
#include "tools/math.hpp"

using namespace scene;

AnimatedSprite::AnimatedSprite(bool isLerpable)
    : baseClass(isLerpable)
{
    setDetectable(false);
}

AnimatedSprite::~AnimatedSprite()
{
    if (m_spriterEntity != nullptr)
        delete m_spriterEntity;
}

//-------------------//
//----- Routine -----//

void AnimatedSprite::drawInternal(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (m_spriterEntity == nullptr);

    m_spriterEntity->setPosition({getPosition().x, getPosition().y});
    m_spriterEntity->setScale({scale().x, scale().y});
    m_spriterEntity->setAngle(getRotation() * M_PI / 180.f);

    // Forcing immediate refresh to current parameters
    m_spriterEntity->reprocessCurrentTime();

    // TODO This does not take states shader in account?
    // And we might want to pass the target here.
    m_spriterEntity->render();
}

void AnimatedSprite::updateRoutine(const sf::Time& dt)
{
    returnif (m_spriterEntity == nullptr);

    if (m_started) {
        forward(dt);
        m_spriterEntity->playAllTriggers();
    }
}

//---------------------//
//----- Animation -----//

void AnimatedSprite::load(const std::string& id)
{
    // Note: we're currently using only one entity per model
    auto& model = Application::context().animations.getModel(id);
    if (m_spriterEntity != nullptr) delete m_spriterEntity;
    m_spriterEntity = model.getNewEntityInstance(0);

    // The first animation is loaded
    restart();

    m_spriterEntity->setTiltColor(m_tiltColor);
}

void AnimatedSprite::select(const std::wstring& animationName)
{
    returnif ((m_spriterEntity == nullptr) || (m_currentAnimationName == animationName));

    // Note: If the animation does not exists, it is just ignored
    m_spriterEntity->setCurrentAnimation(toString(animationName));
    m_currentAnimationName = animationName;

    restart();
}

void AnimatedSprite::forward(const sf::Time& offset)
{
    returnif (m_spriterEntity == nullptr);

    auto timeElapsed = offset.asSeconds() * 1000.f;
    m_spriterEntity->setTimeElapsed(timeElapsed);
}

void AnimatedSprite::restart()
{
    m_started = true;
    if (m_spriterEntity != nullptr)
        m_spriterEntity->setCurrentTime(0.);

    // Getting the current hitbox if any
    auto hitbox = m_spriterEntity->getObjectInstance("hitbox");
    if (hitbox != nullptr) {
        m_hitbox.width  = hitbox->getSize().x;
        m_hitbox.height = hitbox->getSize().y;
        m_hitbox.left   = hitbox->getPosition().x;
        m_hitbox.top    = hitbox->getPosition().y;
    }
    // Setting the hixbox to some default
    else {
        m_hitbox.width  = 50.f;
        m_hitbox.height = 50.f;
        m_hitbox.left   = -25.f;
        m_hitbox.top    = -25.f;
    }
}

//-------------------------//
//----- Extra control -----//

bool AnimatedSprite::started() const
{
    returnif (m_spriterEntity == nullptr) false;
    return m_started && m_spriterEntity->isPlaying;
}

void AnimatedSprite::setTiltColor(const sf::Color& color)
{
    returnif (m_tiltColor == color);
    m_tiltColor = color;

    returnif (m_spriterEntity == nullptr);
    m_spriterEntity->setTiltColor(m_tiltColor);
}
