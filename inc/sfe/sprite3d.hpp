#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <string>

namespace sfe
{
    //! A sf::Sprite with 3D options.
    /*!
     *  This has been taken from https://github.com/Hapaxia/SelbaWard
     *  and was mainly created by M.J.Silk.
     */

    class Sprite3D final : public sf::Drawable, public sf::Transformable
    {
    public:

        // Constructor.
        Sprite3D();

        // Default destructor.
        ~Sprite3D() = default;

        //-------------------------------//
        //! @name Compatibilty functions
        //! @{

        void setTexture(const sf::Texture& texture, bool resetRect = false, bool resetBackOffset = false);
        void setTextureRect(const sf::IntRect& rectangle);
        void setColor(const sf::Color& color);

        const sf::Texture* getTexture() const;
        sf::IntRect getTextureRect() const;
        sf::Color getColor() const;
        sf::FloatRect getLocalBounds() const;
        sf::FloatRect getGlobalBounds() const;

        //! @}

        //------------------//
        //! @name Back face
        //! @{

        void setBackTexture(const sf::Texture& texture, bool resetOffset = false);
        void setBackFlipEnabled(bool flipBack);
        void setTextureOffset(sf::Vector2i textureOffset);
        void setBackTextureOffset(sf::Vector2i backTextureOffset);

        const sf::Texture* getBackTexture() const;
        bool getBackFlipEnabled() const;
        sf::Vector2i getTextureOffset() const;
        sf::Vector2i getBackTextureOffset() const;

        //! @}

        //--------------------//
        //! @name 3D rotation
        //! @{

        void setPitch(float pitch); // rotation around the x axis
        void setYaw(float yaw);     // rotation around the y axis
        void setRoll(float roll);   // rotation around the z axis (this is the usual 2D rotation)
        void setRotation(float rotation); // supplied as the 3d rotation method overrides the sf::Transformable rotation method
        void setRotation(sf::Vector3f rotation); // set pitch, yaw, and roll at once
        void setRotation3d(sf::Vector3f rotation); // set pitch, yaw, and roll at once
        void setOriginZ(float originZ); // sets the origin's z position, which is taken into account when rotating
        void setOrigin(sf::Vector2f origin); // supplied as the 3d origin method overrides the sf::Transformable origin method: sets the 2D origin position. also resets origin's z position to zero
        void setOrigin(sf::Vector3f origin); // sets the 3D origin position
        void setOrigin3D(sf::Vector3f origin); // sets the 3D origin position

        float getPitch() const;
        float getYaw() const;
        float getRoll() const;
        sf::Vector3f getRotation3d() const;
        sf::Vector3f getOrigin3d() const;
        float getMostExtremeAngle() const; // most extreme angle of pitch and yaw. ranges from 0 to 90

        //! @}

        //-------------------//
        //! @name Mesh setup
        //! @{

        void reserveMeshDensity(unsigned int meshDensity); // allow an expected maximum mesh density to be reserved in advance
        void setMeshDensity(unsigned int meshDensity);
        void setDynamicSubdivisionEnabled(bool enabled = true);
        void setDynamicSubdivisionRange(unsigned int maximum, unsigned int minimum = 0u);
        void setSubdivision(const unsigned int subdivision) const; // required to be const to allow dynamic subdivision
        void setNumberOfPoints(unsigned int numberOfPoints); // provided for convenience (sets number of points before any subdivision)
        void setNumberOfQuads(unsigned int numberOfPoints); // provided for convenience (sets number of apparent quads before any subdivision)
        void minimalMesh();

        unsigned int getMeshDensity() const;
        unsigned int getSubdividedMeshDensity() const;
        unsigned int getSubdivision() const;
        bool getDynamicSubdivisionEnabled() const;

        //! @}

        //-----------------//
        //! @name 3D setup
        //! @{

        // depth controls the amount of the apparent depth of the 3D effect.
        // higher values give a more extreme depth effect but more visible texture distortion
        // higher values give a more subtle depth effect but less visible texture distortion
        float getDepth() const;

        void setDepth(float depth);

        //! @}

    protected:

        void createPointGrid() const; // needs to be const to allow dynamic subdivision

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void updateTransformedPoints() const;
        void updateVertices() const;
        void updateGlobalCorners() const;

        unsigned int getPointIndexForVertexIndex(unsigned int vertexIndex, bool invertPointX = false) const;
        unsigned int getNumberOfVerticesNeededForCurrentSubdividedMeshDensity() const;
        float linearInterpolation(float from, float to, float alpha) const;
        float mod(float numerator, float denominator) const;
        float min(float a, float b) const;
        float max(float a, float b) const;
        sf::Vector2i abs(const sf::Vector2i& vector) const;

    private:

        const float m_depthToShallownessConversionNumerator = 10000.f;

        float m_pitch = 0.f;
        float m_yaw = 0.f;
        float m_depth = 10.f; // even though m_shallowness is the one that actually gets used internally, this is stored as a form of cache to return through getDepth() to avoid the unnecessary division in a getter
        float m_shallowness = 0.f;
        unsigned int m_meshDensity = 0u;
        bool m_flipBack = false; // flips the back's texture coordinates so that it shows the right way around

        // texture
        const sf::Texture* m_pTexture = nullptr;
        const sf::Texture* m_pBackTexture = nullptr;
        sf::Vector2i m_size;
        sf::Vector2i m_textureOffset;
        sf::Vector2i m_backTextureOffset;

        // for dynamic subdivision based on angle
        bool m_useDynamicSubdivision = false;
        unsigned int m_minSubdivision = 1u;
        unsigned int m_maxSubdivision = 4u;

        // need to be mutable to allow dynamic subdivision
        mutable unsigned int m_subdivision = 0u;
        mutable unsigned int m_subdividedMeshDensity = 0u; // stored as a cache to avoid unnecessary power calculations
        mutable std::vector<sf::Vector3f> m_points;

        // need to be mutable to allow modification through draw call
        mutable std::vector<sf::Vector2f> m_transformedPoints;
        mutable sf::Vector3f m_origin;
        mutable std::vector<sf::Vertex> m_vertices;
        mutable std::vector<float> m_compactTransformMatrix;
        mutable bool m_isBackFacing = false;

        // corners' global positions (mutable as they are automatically updated when the points are transformed)
        mutable sf::Vector2f m_topLeft;
        mutable sf::Vector2f m_topRight;
        mutable sf::Vector2f m_bottomLeft;
        mutable sf::Vector2f m_bottomRight;
    };

}
