#include "nui/frame.hpp"

#include "core/application.hpp"
#include "tools/tools.hpp"

using namespace nui;

Frame::Frame()
{
    setDetectable(false);

    // Corners

    addPart(&m_topLeft);
    addPart(&m_topRight);
    addPart(&m_bottomLeft);
    addPart(&m_bottomRight);

    const auto& topLeftTexture = Application::context().textures.get("nui/frame/top_left");

    m_topLeft.setTexture(topLeftTexture);
    m_topRight.setTexture(topLeftTexture);
    m_bottomLeft.setTexture(topLeftTexture);
    m_bottomRight.setTexture(topLeftTexture);

    m_topRight.setScale(-1.f, 1.f);
    m_bottomLeft.setScale(1.f, -1.f);
    m_bottomRight.setScale(-1.f, -1.f);

    m_cornerSize = sf::v2f(topLeftTexture.getSize());

    // Borders

    addPart(&m_top);
    addPart(&m_left);
    addPart(&m_right);
    addPart(&m_bottom);

    const auto& topTexture = Application::context().textures.get("nui/frame/top");
    const auto& leftTexture = Application::context().textures.get("nui/frame/left");

    m_top.setTexture(&topTexture);
    m_left.setTexture(&leftTexture);
    m_right.setTexture(&leftTexture);
    m_bottom.setTexture(&topTexture);

    m_right.setScale(-1.f, 1.f);
    m_bottom.setScale(1.f, -1.f);

    // Fill
    addPart(&m_fill);

    const auto& fillTexture = Application::context().textures.get("nui/frame/fill");

    m_fill.setTexture(&fillTexture);
}

//-------------------//
//----- Routine -----//

void Frame::onChildDetached(scene::Entity&)
{
    m_content = nullptr;
    updateSize();
}

void Frame::onSizeChanges()
{
    refreshFrame();
}

void Frame::onChildSizeChanges(scene::Entity& child)
{
    returnif (m_content != &child);
    updateSize();
}

void Frame::updateSize()
{
    if (m_content == nullptr)
        setSize(2.f * m_cornerSize);
    else
        setSize(m_content->size() + 2.f * m_cornerSize);
}

//------------------------------//
//----- Content management -----//

void Frame::setContent(scene::Entity& entity)
{
    if (m_content != nullptr) detachChild(*m_content);
    m_content = &entity;
    attachChild(*m_content);
    updateSize();
}

//---------------//
//----- ICU -----//

void Frame::refreshFrame()
{
    m_topLeft.setPosition(0.f, 0.f);
    m_topRight.setPosition(size().x, 0.f);
    m_bottomLeft.setPosition(0.f, size().y);
    m_bottomRight.setPosition(size().x, size().y);

    m_top.setPosition(m_cornerSize.x, 0.f);
    m_left.setPosition(0.f, m_cornerSize.y);
    m_right.setPosition(size().x, m_cornerSize.y);
    m_bottom.setPosition(m_cornerSize.x, size().y);

    m_fill.setPosition(m_cornerSize);

    m_top.setSize({size().x - 2.f * m_cornerSize.x, m_cornerSize.y});
    m_left.setSize({m_cornerSize.x, size().y - 2.f * m_cornerSize.y});
    m_right.setSize({m_cornerSize.x, size().y - 2.f * m_cornerSize.y});
    m_bottom.setSize({size().x - 2.f * m_cornerSize.x, m_cornerSize.y});

    m_fill.setSize(size() - 2.f * m_cornerSize);

    returnif (m_content == nullptr);

    m_content->setLocalPosition(m_cornerSize);
}
