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

void AnimatedSprite::onScaleChanges()
{
    refreshSpriterEntityTransform();
}

void AnimatedSprite::drawInternal(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (m_spriterEntity == nullptr);
    states.transform = getTransform();
    m_spriterEntity->render(target, states, m_tiltColor);
}

void AnimatedSprite::updateRoutine(const sf::Time& dt)
{
    returnif (m_spriterEntity == nullptr);

    // Animate
    if (m_started) {
        forward(dt);
        m_spriterEntity->playAllTriggers();
    }

    // Update hitbox
    if (m_hasHitbox) {
        auto hitbox = m_spriterEntity->getObjectInstance("hitbox");
        m_hitbox.width  = hitbox->getSize().x * hitbox->getScale().x;
        m_hitbox.height = hitbox->getSize().y * hitbox->getScale().y;
        m_hitbox.left   = hitbox->getPosition().x;
        m_hitbox.top    = hitbox->getPosition().y;
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
}

void AnimatedSprite::select(const std::string& animationName)
{
    returnif ((m_spriterEntity == nullptr) || (m_currentAnimationName == animationName));

    // Note: If the animation does not exists, it is just ignored
    m_spriterEntity->setCurrentAnimation(animationName);
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
    returnif (m_spriterEntity == nullptr);

    refreshSpriterEntityTransform();
    m_spriterEntity->setCurrentTime(0.);
    m_spriterEntity->isPlaying = true;

    // Setting the hitbox to some default if none
    m_hasHitbox = (m_spriterEntity->getObjectInstance("hitbox") != nullptr);
    if (!m_hasHitbox) {
        m_hitbox.width  = 50.f;
        m_hitbox.height = 50.f;
        m_hitbox.left   = -25.f;
        m_hitbox.top    = -25.f;
    }
}

//------------------//
//----- Hitbox -----//

sf::FloatRect AnimatedSprite::findBox(const std::string& objectName) const
{
    sf::FloatRect boxBounds;
    auto box = m_spriterEntity->getObjectInstance(objectName);

    if (box != nullptr) {
        boxBounds.width  = box->getSize().x * box->getScale().x;
        boxBounds.height = box->getSize().y * box->getScale().y;
        boxBounds.left   = box->getPosition().x;
        boxBounds.top    = box->getPosition().y;
    }

    return boxBounds;
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
    m_tiltColor = color;
}

//---------------//
//----- ICU -----//

void AnimatedSprite::refreshSpriterEntityTransform()
{
    returnif (m_spriterEntity == nullptr);

    m_spriterEntity->setScale({scale().x, scale().y});
}
