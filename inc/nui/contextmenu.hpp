#pragma once

#include "nui/object.hpp"

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

#include <sstream>
#include <functional>

namespace nui
{
    class ContextMenu : public Object
    {
        typedef Object baseClass;

    public:
        typedef std::function<void()> Callback;

        ContextMenu();
        virtual ~ContextMenu() {}

        // Virtual
        void init() override;
        void update() override;
        void handleMouseEvent(const sf::Event& event, const sf::Vector2f& relPos) override;

        // Choices
        void clearChoices();
        void setTitle(const std::wstring& title);
        void addChoice(const std::wstring& text, Callback callback = nullptr);

    protected:
        // Mouse events
        virtual void handleMousePressed(const sf::Event& event, const sf::Vector2f& relPos);
        virtual void handleMouseMoved(const sf::Event& event, const sf::Vector2f& relPos);
        virtual void handleMouseLeft();

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

