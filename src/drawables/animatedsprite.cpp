#include "drawables/animatedsprite.hpp"

#include "core/application.hpp"

using namespace sfe;

AnimatedSprite::AnimatedSprite()
    : m_looping(true)
    , m_started(true)
{
    Application::context().animations.push(this);
}

AnimatedSprite::~AnimatedSprite()
{
    m_entities.clear();
    Application::context().animations.pop(this);
}

void AnimatedSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    // Transform from sf::Tranformable
    states.transform *= getTransform();

    // Drawing
    for (const auto& entity : m_entities)
        entity->draw(getPosition().x, getPosition().y, getRotation(), getScale().x, getScale().y);
}

void AnimatedSprite::update(const sf::Time& dt)
{
    if (m_started) {
        for (const auto& entity : m_entities) {
            entity->update(dt.asMilliseconds());

            if (!m_looping && entity->time >= entity->getAnimation(m_number)->length)
                m_started = false;
        }
    }
}

//----- Controls animation -----//

void AnimatedSprite::load(Animations::ID id, int number)
{
    auto& data = Application::context().animations.getData(id);
    auto& fs = Application::context().animations.getFileSystem(id);
    m_number = number;

    // Loading animation
    m_entities.clear();
    for (const auto& entityInfo : data.entities) {
        auto entity = std::unique_ptr<scml::Entity>(new scml::Entity(&data, entityInfo.first));
        entity->setScreen(&Application::context().window);
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

//----- Updates -----//

void AnimatedSprite::refresh()
{
    for (const auto& entity : m_entities)
        entity->getAnimation(m_number)->looping = (m_looping)? "true" : "false";
}
