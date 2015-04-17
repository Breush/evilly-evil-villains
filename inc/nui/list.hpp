#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"

#include <initializer_list>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace nui
{
    class List final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:
        List();
        virtual ~List() = default;

        // Columns
        void setColumns(const std::initializer_list<std::wstring>& columns);
        void setColumnFillClip(uint index, bool fill, bool clip);
        uint getColumnWidthMax(uint index);
        uint getColumnWidthHint();

        // Lines
        void addLine(const std::initializer_list<std::wstring>& values);

    protected:
        // Virtual
        void update() override;
        void refreshDisplay() final;

        // Events
        void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) override;
        void handleMouseLeft() override;

        // Updates
        void updateColumnInfos();

        // Columns info
        struct ColumnInfo {
            sf::Text text;
            uint x;
            uint width;
            bool fill;
            bool clip;
        };

        // Lines info
        struct LineInfo {
            sf::Text text;
            uint textWidth;
            uint textHeight;
            sf::FloatRect clippingRect;
        };

        // Changes
        void changedSelectedLine();

        // Focus interactions
        void setFocusedLine(uint line);
        void setSelectionRect(const sf::FloatRect& focusRect);

        // Params
        PARAMG(float, m_lineHeight, lineHeight)
        PARAMGSU(uint, m_selectedLine, selectedLine, setSelectedLine, changedSelectedLine);

    private:
        // Columns
        std::vector<ColumnInfo> m_columns;

        // Lines
        std::vector<std::vector<LineInfo>> m_lines;
        uint m_linesCount = 0u;

        // Decorum
        std::vector<sfe::HLine> m_hBorders;
        std::vector<sfe::VLine> m_vBorders;
        sf::RectangleShape m_selectionHighlight;
    };
}

