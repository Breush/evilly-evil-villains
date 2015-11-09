#include "scene/wrappers/animatedsprite.hpp"

#include "core/application.hpp"
#include "tools/platform-fixes.hpp" // make_unique
#include "tools/string.hpp"
#include "tools/tools.hpp"
#include "tools/debug.hpp"

using namespace scene;

AnimatedSprite::AnimatedSprite(bool isLerpable)
    : baseClass(isLerpable)
    , m_looping(true)
    , m_started(true)
{
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
    m_spriterEntity->setAngle(getRotation());

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

        if (!m_looping && m_spriterEntity->getCurrentTime() >= m_length - dt.asMilliseconds())
            m_started = false;
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
    m_length = m_spriterEntity->getCurrentAnimationLength();
    restart();

    // TODO
    /* m_spriterEntity->setTiltColor(m_tiltColor); */
}

void AnimatedSprite::select(const std::wstring& animationName)
{
    returnif (m_spriterEntity == nullptr);

    m_spriterEntity->setCurrentAnimation(toString(animationName));

    m_length = m_spriterEntity->getCurrentAnimationLength();
    restart();

    // TODO What happens if the animation does not exists in this model?
    // And how to detect it?
}

void AnimatedSprite::forward(const sf::Time& offset)
{
    returnif (m_spriterEntity == nullptr);

    auto timeElapsed = offset.asMilliseconds();
    m_spriterEntity->setTimeElapsed(timeElapsed);
}

void AnimatedSprite::restart()
{
    m_started = true;
    m_spriterEntity->setCurrentTime(0.);
    refreshFromLooping();
}

//-------------------------//
//----- Extra control -----//

void AnimatedSprite::refreshFromLooping()
{
    // TODO This does not work
    // m_spriterEntity->setCurrentAnimationLooping(m_looping);
}

void AnimatedSprite::setTiltColor(const sf::Color& color)
{
    returnif (m_tiltColor == color);
    m_tiltColor = color;

    returnif (m_spriterEntity == nullptr);
    /* m_spriterEntity->setTiltColor(m_tiltColor); */
}
