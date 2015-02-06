#pragma once

#include "nui/object.hpp"
#include "tools/int.hpp"
#include "sfe/line.hpp"

#include <initializer_list>
#include <SFML/Graphics/Text.hpp>

namespace nui
{
    class List : public Object
    {
        typedef Object baseClass;

    public:
        List();
        virtual ~List() {}

        // Virtual
        void init() override;
        void update() override;
        //void handleKeyboardEvent(const sf::Event& event) override;
        //void handleJoystickEvent(const sf::Event& event) override;

        // Columns
        void setColumns(const std::initializer_list<std::wstring>& columns);
        void setColumnFillClip(uint index, bool fill, bool clip);
        uint getColumnWidthMax(uint index);
        uint getColumnWidthHint();

        // Lines
        void addLine(const std::initializer_list<std::wstring>& values);

    protected:
        // Mouse events
        void handleMouseButtonPressed(const sf::Mouse::Button& mouseButton, const sf::Vector2f& mousePos) override;
        void handleMouseMoved(const sf::Vector2f& mousePos) override;
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
            sf::IntRect clippingRect;
        };

        // Changes
        void changedSize() override;
        void changedSelectedLine();

        // Focus interactions
        bool ownsFocus() override { return true; }
        void setFocusedLine(uint line);

        // Params
        PARAMG(uint, m_hPadding, hPadding)
        PARAMG(uint, m_vPadding, vPadding)
        PARAMG(uint, m_borderThick, borderThick)
        PARAMG(uint, m_sbWidth, sbWidth)
        PARAMG(uint, m_lineHeight, lineHeight)
        PARAMG(uint, m_linesCount, linesCount)
        PARAMGSU(uint, m_selectedLine, selectedLine, setSelectedLine, changedSelectedLine);

    private:
        // Columns
        std::vector<ColumnInfo> m_columns;

        // Lines
        std::vector<std::vector<LineInfo>> m_lines;

        // Decorum
        std::vector<sfe::HLine> m_hBorders;
        std::vector<sfe::VLine> m_vBorders;
    };
}

