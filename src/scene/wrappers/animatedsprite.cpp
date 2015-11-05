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
    delete m_spriterEntity;
}

//-------------------//
//----- Routine -----//

void AnimatedSprite::onTransformChanges()
{
    returnif (m_spriterEntity == nullptr);

    m_spriterEntity->setPosition({getPosition().x, getPosition().y});
    m_spriterEntity->setScale({getScale().x, getScale().y});
    m_spriterEntity->setAngle(getRotation());
}

void AnimatedSprite::drawInternal(sf::RenderTarget& target, sf::RenderStates states) const
{
    returnif (m_spriterEntity == nullptr);

    /*
    m_spriterEntity->setScale(SpriterEngine::point(getScale(), getScale()));
    inst->setPosition(SpriterEngine::point(rand() % window.getSize().x, rand() % window.getSize().y));
    inst->setAngle(SpriterEngine::toRadians(rand() % 360));*/

    m_spriterEntity->render();

    /*
    // TODO This does not take states shader in account?
    // TODO This probably should use states.tranform info for position/rotation/scale.

    // Drawing all parts of animation
    // because scml::Entity can not be managed by scene::Graph
    for (const auto& entity : m_entities) {
        entity->setScreen(&target);
        entity->draw(getPosition().x, getPosition().y, getRotation(), scale().x, scale().y);
    }*/
}

void AnimatedSprite::updateRoutine(const sf::Time& dt)
{
    returnif (m_spriterEntity == nullptr);

    // returnif (!m_started);
    forward(dt);

    // Sounds etc.
    m_spriterEntity->playAllTriggers();
}

//---------------------//
//----- Animation -----//

void AnimatedSprite::load(const std::string& id)
{
    // Note: we're currently using only one entity per model
    auto& model = Application::context().animations.getModel(id);
    // TODO Use smart pointer in the interface?
    delete m_spriterEntity;
    m_spriterEntity = model.getNewEntityInstance(0);

    // Reaffects values
    // TODO Factor with onTransformChanges()
    m_spriterEntity->setPosition({getPosition().x, getPosition().y});
    m_spriterEntity->setScale({getScale().x, getScale().y});
    m_spriterEntity->setAngle(getRotation());

    /*
        entity->setTiltColor(m_tiltColor);
    refresh();*/
}

void AnimatedSprite::select(const std::wstring& animationName)
{
    returnif (m_spriterEntity == nullptr);

    m_spriterEntity->setCurrentAnimation(toString(animationName));
    /*
    returnif (m_entities.empty());

    const auto& firstEntity = *m_entities.front();
    auto animation = firstEntity.getAnimation(animationName);
    wassert(animation != nullptr, L"Requested animation '" << animationName
                                  << "' not found in entity '" << firstEntity.name << L"'.");

    if (animation->id != m_number) {
        m_number = animation->id;
        refresh();
        restart();
    }*/
}

void AnimatedSprite::forward(const sf::Time& offset)
{
    returnif (m_spriterEntity == nullptr);
    m_spriterEntity->setTimeElapsed(offset.asMilliseconds());

    /*
    for (const auto& entity : m_entities) {
        entity->updateAnimation(offset.asMilliseconds());

        if (!m_looping && entity->time >= entity->getAnimation(m_number)->length)
            m_started = false;
    }*/
}

void AnimatedSprite::restart()
{
    /*
    m_started = true;
    for (const auto& entity : m_entities)
        entity->startAnimation(m_number);*/
}

//-------------------------//
//----- Extra control -----//

void AnimatedSprite::setTiltColor(const sf::Color& color)
{
    /*
    returnif (m_tiltColor == color);
    m_tiltColor = color;

    for (const auto& entity : m_entities)
        entity->setTiltColor(m_tiltColor);*/
}

//-----------------------------------//
//----- Internal change updates -----//

void AnimatedSprite::refresh()
{
    returnif (m_spriterEntity == nullptr);

    // Can't I override setLooping ?
    //m_spriterEntity->getEntity(0)->getAnimation(m_number)->
    /*
    for (const auto& entity : m_entities)
        entity->getAnimation(m_number)->looping = (m_looping)? L"true" : L"false";*/
}
