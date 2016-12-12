#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>

#include <vector>

namespace sfe
{
    //! A parametrable spline.

    class Spline : public sf::Drawable
    {
    public:
        struct Vertex
        {
            sf::Vector2f position;
            sf::Vector2f frontHandle; // offset from position
            sf::Vector2f backHandle; // offset from position
            Vertex() {}
            Vertex(sf::Vector2f newPosition) : position(newPosition) {}
        };

        Spline(unsigned int vertexCount = 0, sf::Vector2f initialPosition = { 0.f, 0.f });
        void update();

        Vertex& operator[] (unsigned int index); // direct access to the spline's vertices (sw::Spline::Vertex) using the [] operator. no checks are performed. using with an invalid index results in undefined behaviour

        const unsigned int getVertexCount() const;
        const unsigned int getLastVertexIndex() const;
        const float getLength() const; // returns the length of (linear) spline - the sum of the lengths of all of the lines between consecutive vertices
        const float getInterpolatedLength() const; // returns the actual length of the spline - the sum of the lengths of all of the line segments drawn

        void reserveVertices(unsigned int numberOfVertices);

        void addVertices(const std::vector<sf::Vector2f>& positions);
        void addVertices(unsigned int index, const std::vector<sf::Vector2f>& positions); // inserts the new vertices before the vertex at the specified index
        void addVertices(unsigned int numberOfVertices, sf::Vector2f position = { 0.f, 0.f });
        void addVertices(unsigned int numberOfVertices, unsigned int index, sf::Vector2f position = { 0.f, 0.f }); // inserts the new vertices before the vertex at the specified index
        void addVertex(sf::Vector2f position = { 0.f, 0.f });
        void addVertex(unsigned int index, sf::Vector2f position = { 0.f, 0.f }); // inserts the new vertex before the vertex at the specified index
        void removeVertex(unsigned int index);
        void removeVertices(unsigned int index, unsigned int numberOfVertices = 0u); // if numberOfvertices is zero (the default), removes all vertices from specified index until the end
        void reverseVertices();

        void setPosition(unsigned int index, sf::Vector2f position = { 0.f, 0.f });
        void setPositions(unsigned int index, unsigned int numberOfVertices = 0u, sf::Vector2f position = { 0.f, 0.f }); // if numberOfvertices is zero (the default), sets positions of all vertices from specified index until the end
        void setPositions(const std::vector<sf::Vector2f>& positions, unsigned int index = 0u);
        const sf::Vector2f getPosition(unsigned int index) const;

        void setFrontHandle(unsigned int index, sf::Vector2f offset);
        const sf::Vector2f getFrontHandle(unsigned int index) const;

        void setBackHandle(unsigned int index, sf::Vector2f offset);
        const sf::Vector2f getBackHandle(unsigned int index) const;

        void resetHandles(unsigned int index = 0u, unsigned int numberOfVertices = 0u); // if numberOfvertices is zero (the default), reset handles for all vertices from specified index until the end. if no index is specified, all handles are reset

        void smoothHandles();

        void setHandlesVisible(bool handlesVisible = true);
        const bool getHandlesVisible() const;

        void setColor(sf::Color color);
        const sf::Color getColor() const;

        void setInterpolationSteps(unsigned int interpolationSteps);
        const unsigned int getInterpolationSteps() const;

        void setHandleMirrorLockEnabled(bool enableHandleMirrorLock = true);
        void setHandleAngleLockEnabled(bool enableHandleAngleLock = true);

        void setBezierInterpolationEnabled(bool enableBezierInterpolation = true);
        const bool getBezierInterpolationEnabled() const;

        void setPrimitiveType(sf::PrimitiveType primitiveType);
        const sf::PrimitiveType getPrimitiveType() const;

    protected:

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        const bool priv_isValidVertexIndex(unsigned int vertexIndex) const;

    private:

        std::vector<Vertex> m_vertices;
        sf::Color m_color = sf::Color::White;

        std::vector<sf::Vertex> m_sfmlVertices;
        sf::PrimitiveType m_primitiveType;
        unsigned int m_interpolationSteps = 0u;
        bool m_useBezier = false;

        std::vector<sf::Vertex> m_handlesVertices;
        bool m_showHandles = false;
        bool m_lockHandleMirror = true;
        bool m_lockHandleAngle = true;
    };

    inline Spline::Vertex& Spline::operator[] (const unsigned int index)
    {
        return m_vertices[index];
    }

    inline const unsigned int Spline::getVertexCount() const
    {
        return m_vertices.size();
    }

    inline const unsigned int Spline::getLastVertexIndex() const
    {
        return m_vertices.size() - 1u;
    }

    inline const bool Spline::getHandlesVisible() const
    {
        return m_showHandles;
    }

    inline const sf::Color Spline::getColor() const
    {
        return m_color;
    }

    inline const unsigned int Spline::getInterpolationSteps() const
    {
        return m_interpolationSteps;
    }

    inline const sf::PrimitiveType Spline::getPrimitiveType() const
    {
        return m_primitiveType;
    }

    inline const bool Spline::getBezierInterpolationEnabled() const
    {
        return m_useBezier;
    }
}

