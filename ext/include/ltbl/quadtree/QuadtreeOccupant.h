#pragma once

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <ltbl/tools/Math.h>

#include <memory>
#include <array>
#include <unordered_set>

namespace ltbl {
    class QuadtreeOccupant {
        friend class Quadtree;
        friend class QuadtreeNode;
        friend class DynamicQuadtree;
        friend class StaticQuadtree;

    private:
        class QuadtreeNode* _pQuadtreeNode;
        class Quadtree* _pQuadtree;

    public:
        QuadtreeOccupant()
            : _pQuadtreeNode(nullptr), _pQuadtree(nullptr)
        {}

        void quadtreeUpdate();
        void quadtreeRemove();

        virtual sf::FloatRect getAABB() const = 0;
    };
}
