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
        using baseClass = scene::Entity;

    public:
        using Callback = std::function<void()>;

        ContextMenu();
        virtual ~ContextMenu() = default;

        // Events
        void handleGlobalEvent(const sf::Event& event);
        void handleMouseButtonPressed(const sf::Mouse::Button& button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Choices
        void clearChoices();
        void setTitle(const std::wstring& title);
        void addChoice(const std::wstring& text, Callback callback = nullptr);

    protected:
        // Updates
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
        sf::RectangleShape m_background;

        // Choices
        sf::Text m_title;
        std::vector<ChoiceInfo> m_choices;
    };
}

