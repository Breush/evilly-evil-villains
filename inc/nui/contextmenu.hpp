#pragma once

#include "scene/entity.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <sstream>
#include <functional>

namespace nui
{
    class ContextMenu : public scene::Entity
    {
        typedef scene::Entity baseClass;

    public:
        typedef std::function<void()> Callback;

        ContextMenu();
        virtual ~ContextMenu() {}

        // Events
        void handleGlobalEvent(const sf::Event& event) override;
        void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos) override;
        void handleMouseLeft() override;

        // Choices
        void clearChoices();
        void setTitle(const std::wstring& title);
        void addChoice(const std::wstring& text, Callback callback = nullptr);

    protected:
        // Virtual
        void update() override;
        void updateSize();

        // Params
        PARAMG(uint, m_padding, padding)
        PARAMG(uint, m_choiceHeight, choiceHeight)

        // Choices
        uint choiceFromCoords(const sf::Vector2f& coords) const;

        // Keep infos
        struct ChoiceInfo {
            sf::Text text;
            Callback callback;
        };

    private:
        // Decorum
        sf::RectangleShape m_bg; // TODO To be a sf::Sprite for looks

        // Choices
        sf::Text m_title;
        std::vector<ChoiceInfo> m_choices;
    };
}

