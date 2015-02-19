#include "sfe/animatedsprite.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

using namespace sfe;

AnimatedSprite::AnimatedSprite()
    : m_looping(true)
    , m_started(true)
{
    Application::context().animations.push(this);
}

AnimatedSprite::~AnimatedSprite()
{
    Application::context().animations.pop(this);
}

//-------------------//
//----- Routine -----//

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Drawing all parts of animation
    // because scml::Entity can not be managed by scene::Graph
    for (const auto& entity : m_entities) {
        entity->setScreen(&target);
        entity->draw(getPosition().x, getPosition().y, getRotation(), getScale().x, getScale().y);
    }

    baseClass::draw(target, states);
}

void AnimatedSprite::updateAnimation(const sf::Time& dt)
{
    returnif (!m_started);

    for (const auto& entity : m_entities) {
        entity->updateAnimation(dt.asMilliseconds());

        if (!m_looping && entity->time >= entity->getAnimation(m_number)->length)
            m_started = false;
    }
}

void AnimatedSprite::refresh()
{
    for (const auto& entity : m_entities)
        entity->getAnimation(m_number)->looping = (m_looping)? L"true" : L"false";
}

//------------------------------//
//----- Controls animation -----//

void AnimatedSprite::load(AnimationID id, int number)
{
    auto& data = Application::context().animations.getData(id);
    auto& fs = Application::context().animations.getFileSystem(id);
    m_number = number;

    // Removing if previous animation
    m_entities.clear();

    // Loading animation
    for (const auto& entityInfo : data.entities) {
        auto entity = std::unique_ptr<scml::Entity>(new scml::Entity(&data, entityInfo.first));
        entity->setFileSystem(&fs);
        m_entities.push_back(std::move(entity));
    }

    // Refresh all entities
    refresh();
}

void AnimatedSprite::restart()
{
    for (const auto& entity : m_entities)
        entity->startAnimation(m_number);
}
