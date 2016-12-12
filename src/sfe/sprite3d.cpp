#include "sfe/sprite3d.hpp"

#include <cmath>

using namespace sfe;

Sprite3D::Sprite3D()
    : m_shallowness(m_depthToShallownessConversionNumerator / m_depth)
    , m_points(4)
    , m_transformedPoints(4)
    , m_vertices(4)
    , m_compactTransformMatrix(8, 0.f) // was 5. is now 8 to take into account z (for origin's z)
{
}

void Sprite3D::setTextureRect(const sf::IntRect& textureRectangle)
{
    m_textureOffset = sf::Vector2i(textureRectangle.left, textureRectangle.top);
    m_backTextureOffset = m_textureOffset;
    m_size = sf::Vector2i(textureRectangle.width, textureRectangle.height);
    createPointGrid();
    updateTransformedPoints();
    updateVertices();
    updateGlobalCorners();
}

void Sprite3D::setTexture(const sf::Texture& texture, const bool resetRect, const bool resetBackOffset)
{
    if (m_pTexture == nullptr || resetRect)
    {
        m_textureOffset = sf::Vector2i(0, 0);
        m_size = sf::Vector2i(texture.getSize());
        createPointGrid();
        m_vertices.resize(getNumberOfVerticesNeededForCurrentSubdividedMeshDensity());
    }
    if (resetBackOffset)
        m_backTextureOffset = sf::Vector2i(0, 0);
    m_pTexture = &texture;
}

void Sprite3D::setBackTexture(const sf::Texture& texture, const bool resetOffset)
{
    m_pBackTexture = &texture;
    if (m_pBackTexture == nullptr || resetOffset)
        m_backTextureOffset = sf::Vector2i(0, 0);
}

void Sprite3D::setBackFlipEnabled(const bool flipBack)
{
    m_flipBack = flipBack;
}

const sf::Texture* Sprite3D::getTexture() const
{
    return m_pTexture;
}

const sf::Texture* Sprite3D::getBackTexture() const
{
    return m_pBackTexture;
}

bool Sprite3D::getBackFlipEnabled() const
{
    return m_flipBack;
}

sf::Vector2i Sprite3D::getTextureOffset() const
{
    return m_textureOffset;
}

void Sprite3D::setTextureOffset(sf::Vector2i textureOffset)
{
    m_textureOffset = textureOffset;
}

sf::Vector2i Sprite3D::getBackTextureOffset() const
{
    return m_backTextureOffset;
}

void Sprite3D::setBackTextureOffset(sf::Vector2i backTextureOffset)
{
    m_backTextureOffset = backTextureOffset;
}

void Sprite3D::setColor(const sf::Color& color)
{
    for (auto& vertex : m_vertices)
        vertex.color = color;
}

sf::Color Sprite3D::getColor() const
{
    return m_vertices[0].color;
}

float Sprite3D::getPitch() const
{
    return m_pitch;
}

float Sprite3D::getYaw() const
{
    return m_yaw;
}

float Sprite3D::getRoll() const
{
    return getRotation();
}

sf::Vector3f Sprite3D::getRotation3d() const
{
    return{ m_pitch, m_yaw, Transformable::getRotation() };
}

sf::Vector3f Sprite3D::getOrigin3d() const
{
    const sf::Vector2f origin2d{ Transformable::getOrigin() };
    return{ origin2d.x, origin2d.y, m_origin.z };
}

void Sprite3D::setPitch(float pitch)
{
    m_pitch = pitch;
    while (m_pitch > 180.f)
        m_pitch -= 360.f;
    while (m_pitch < -180.f)
        m_pitch += 360.f;
}

void Sprite3D::setYaw(float yaw)
{
    m_yaw = yaw;
    while (m_yaw > 180.f)
        m_yaw -= 360.f;
    while (m_yaw < -180.f)
        m_yaw += 360.f;
}

void Sprite3D::setRoll(float roll)
{
    Transformable::setRotation(roll);
}

void Sprite3D::setRotation(float rotation)
{
    setRoll(rotation);
}

void Sprite3D::setRotation(sf::Vector3f rotation)
{
    setRotation3d(rotation);
}

void Sprite3D::setRotation3d(sf::Vector3f rotation)
{
    setPitch(rotation.x);
    setYaw(rotation.y);
    setRoll(rotation.z);
}

void Sprite3D::setOriginZ(float originZ)
{
    m_origin.z = originZ;
}

void Sprite3D::setOrigin(sf::Vector2f origin)
{
    Transformable::setOrigin(origin);
    m_origin.z = 0.f; // reset origin's z back to zero when setting a 2D origin
}

void Sprite3D::setOrigin(sf::Vector3f origin)
{
    setOrigin3D(origin);
}

void Sprite3D::setOrigin3D(sf::Vector3f origin)
{
    setOrigin(sf::Vector2f(origin.x, origin.y));
    setOriginZ(origin.z);
}

float Sprite3D::getMostExtremeAngle() const
{
    float pitch = std::abs(m_pitch);
    if (pitch > 90.f)
        pitch = 180.f - pitch;
    float yaw = std::abs(m_yaw);
    if (yaw > 90.f)
        yaw = 180.f - yaw;
    return std::max(pitch, yaw);
}

void Sprite3D::setMeshDensity(const unsigned int meshDensity)
{
    m_meshDensity = meshDensity;

    setSubdivision(m_subdivision);
}

unsigned int Sprite3D::getMeshDensity() const
{
    return m_meshDensity;
}

unsigned int Sprite3D::getSubdividedMeshDensity() const
{
    return m_subdividedMeshDensity;
}

void Sprite3D::reserveMeshDensity(const unsigned int meshDensity)
{
    const unsigned int numberOfPointsPerDimension = meshDensity + 2;
    m_points.reserve(numberOfPointsPerDimension * numberOfPointsPerDimension);
    m_transformedPoints.reserve(m_points.size());

    const unsigned int currentSubdividedMeshDensity = m_subdividedMeshDensity;
    m_subdividedMeshDensity = meshDensity;
    m_vertices.reserve(getNumberOfVerticesNeededForCurrentSubdividedMeshDensity());
    m_subdividedMeshDensity = currentSubdividedMeshDensity;
}

void Sprite3D::setDynamicSubdivisionEnabled(const bool enabled)
{
    m_useDynamicSubdivision = enabled;
}

void Sprite3D::setDynamicSubdivisionRange(unsigned int maximum, unsigned int minimum)
{
    if (maximum < minimum)
    {
        unsigned int temp;
        temp = maximum;
        maximum = minimum;
        minimum = temp;
    }
    m_maxSubdivision = maximum;
    m_minSubdivision = minimum;
    reserveMeshDensity(m_maxSubdivision);
}

bool Sprite3D::getDynamicSubdivisionEnabled() const
{
    return m_useDynamicSubdivision;
}

void Sprite3D::setSubdivision(const unsigned int subdivision) const
{
    m_subdivision = subdivision;

    m_subdividedMeshDensity = m_meshDensity;

    for (unsigned int i = 0; i < m_subdivision; ++i)
        m_subdividedMeshDensity = m_subdividedMeshDensity * 2 + 1;

    createPointGrid();
    m_vertices.resize(getNumberOfVerticesNeededForCurrentSubdividedMeshDensity());
}

unsigned int Sprite3D::getSubdivision() const
{
    return m_subdivision;
}

void Sprite3D::setNumberOfPoints(const unsigned int numberOfPoints)
{
    unsigned int root = static_cast<unsigned int>(sqrt(numberOfPoints));
    if (root > 2)
        setMeshDensity(root - 2);
    else
        setMeshDensity(0);
}

void Sprite3D::setNumberOfQuads(const unsigned int numberOfQuads)
{
    unsigned int root = static_cast<unsigned int>(sqrt(numberOfQuads));
    if (root > 1)
        setMeshDensity(root - 1);
    else
        setMeshDensity(0);
}

void Sprite3D::minimalMesh()
{
    m_meshDensity = 0;
    setSubdivision(0);
}

sf::FloatRect Sprite3D::getLocalBounds() const
{
    return sf::FloatRect(sf::Vector2f(0.f, 0.f), sf::Vector2f(abs(m_size)));
}

sf::FloatRect Sprite3D::getGlobalBounds() const
{
    updateTransformedPoints();
    updateGlobalCorners();

    float minX = min(m_topLeft.x, min(m_topRight.x, min(m_bottomLeft.x, m_bottomRight.x)));
    float maxX = max(m_topLeft.x, max(m_topRight.x, max(m_bottomLeft.x, m_bottomRight.x)));
    float minY = min(m_topLeft.y, min(m_topRight.y, min(m_bottomLeft.y, m_bottomRight.y)));
    float maxY = max(m_topLeft.y, max(m_topRight.y, max(m_bottomLeft.y, m_bottomRight.y)));

    return sf::FloatRect(sf::Vector2f(minX, minY), sf::Vector2f(maxX - minX, maxY - minY));
}

void Sprite3D::setDepth(const float depth)
{
    m_depth = depth;
    m_shallowness = m_depthToShallownessConversionNumerator / ((m_depth > -0.000001f && m_depth < 0.000001f) ? 0.000001f : m_depth); // avoid division by zero here but don't change m_depth from being zero
}

float Sprite3D::getDepth() const
{
    return m_depth;
}

void Sprite3D::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (m_pTexture != nullptr)
    {
        updateTransformedPoints();
        updateVertices();
        states.transform *= getTransform();

        if (m_isBackFacing && m_pBackTexture != nullptr)
            states.texture = m_pBackTexture;
        else
            states.texture = m_pTexture;

        target.draw(&m_vertices[0], m_vertices.size(), sf::PrimitiveType::TrianglesStrip, states);
    }
}

void Sprite3D::updateTransformedPoints() const
{
    if (m_useDynamicSubdivision)
        setSubdivision(static_cast<unsigned int>((m_maxSubdivision - m_minSubdivision) * getMostExtremeAngle() / 90.f + m_minSubdivision));

    m_origin = { getOrigin().x, getOrigin().y, m_origin.z };
    const float radiansFromDegreesMultiplier = 0.0174532925f; // pi / 180;
    const float pitchInRadians = m_pitch * radiansFromDegreesMultiplier;
    const float yawInRadians = m_yaw * radiansFromDegreesMultiplier;

    const float cosPitch = cos(pitchInRadians);
    const float sinPitch = sin(pitchInRadians);
    const float cosYaw = cos(yawInRadians);
    const float sinYaw = sin(yawInRadians);

    /*******************************************************
    *          Pitch and Yaw combined matrix               *
    *                                                      *
    *  cosYaw,  sinPitch * sinYaw, -cosPitch * sinYaw, 0,  *
    *  0,       cosPitch,           sinPitch,          0,  *
    *  sinYaw, -sinPitch * cosYaw,  cosPitch * cosYaw, 0,  *
    *  0,       0,                  0,                 1   *
    *******************************************************/

    //m_compactTransformMatrix = { cosYaw, sinYaw, sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw }; // only the five used elements
    m_compactTransformMatrix = { cosYaw, sinYaw, sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw, -cosPitch * sinYaw, sinPitch, cosPitch * cosYaw }; // all eight used elements

    for (unsigned int v = 0; v < m_points.size(); ++v)
    {
        sf::Vector3f point = m_points[v];

        point -= m_origin;
        point =
        {
            /*
            m_compactTransformMatrix[0] * point.x + m_compactTransformMatrix[2] * point.y,
                                                    m_compactTransformMatrix[3] * point.y,
            m_compactTransformMatrix[1] * point.x + m_compactTransformMatrix[4] * point.y
            */
            m_compactTransformMatrix[0] * point.x + m_compactTransformMatrix[2] * point.y + m_compactTransformMatrix[5] * point.z,
                                                    m_compactTransformMatrix[3] * point.y + m_compactTransformMatrix[6] * point.z,
            m_compactTransformMatrix[1] * point.x + m_compactTransformMatrix[4] * point.y + m_compactTransformMatrix[7] * point.z
        }; // apply rotations
        point *= m_shallowness / (m_shallowness + point.z); // apply depth
        point += m_origin;

        m_transformedPoints[v] = sf::Vector2f(point.x, point.y);
    }

    updateGlobalCorners();

    m_isBackFacing = false;
    if (m_pitch < -90.f || m_pitch > 90.f)
        m_isBackFacing = true;
    if (m_yaw < -90.f || m_yaw > 90.f)
        m_isBackFacing = !m_isBackFacing;
}

void Sprite3D::updateVertices() const
{
    sf::Vector2i currentTextureOffset = m_textureOffset;
    if (m_isBackFacing)
        currentTextureOffset = m_backTextureOffset;

    // create a mesh (triangle strip) from the points
    for (unsigned int v = 0; v < m_vertices.size(); ++v)
    {
        const unsigned int pointIndex = getPointIndexForVertexIndex(v);
        const unsigned int texturePointIndex = getPointIndexForVertexIndex(v, m_isBackFacing && m_flipBack);

        // update vertex
        m_vertices[v].position = m_transformedPoints[pointIndex];
        m_vertices[v].texCoords.x = (m_points[texturePointIndex].x * (m_size.x < 0 ? -1 : 1)) + currentTextureOffset.x;
        m_vertices[v].texCoords.y = (m_points[texturePointIndex].y * (m_size.y < 0 ? -1 : 1)) + currentTextureOffset.y;
    }
}

void Sprite3D::updateGlobalCorners() const
{
    m_topLeft = getTransform().transformPoint(m_transformedPoints.front());
    m_topRight = getTransform().transformPoint(*(m_transformedPoints.begin() + m_subdividedMeshDensity + 1));
    m_bottomLeft = getTransform().transformPoint(*(m_transformedPoints.end() - m_subdividedMeshDensity - 2)); // end() - (m_subdividedMeshDensity + 1) - 1
    m_bottomRight = getTransform().transformPoint(m_transformedPoints.back());

}

void Sprite3D::createPointGrid() const
{
    sf::Vector2f leftTop(0.f, 0.f);
    sf::Vector2f rightBottom(abs(m_size));

    const unsigned int numberOfPointsPerDimension = m_subdividedMeshDensity + 2;

    // create a grid of points
    m_points.resize(numberOfPointsPerDimension * numberOfPointsPerDimension);
    for (unsigned int y = 0; y < numberOfPointsPerDimension; ++y)
    {
        for (unsigned int x = 0; x < numberOfPointsPerDimension; ++x)
        {
            m_points[y * numberOfPointsPerDimension + x].x = linearInterpolation(leftTop.x, rightBottom.x, static_cast<float>(x) / (numberOfPointsPerDimension - 1));
            m_points[y * numberOfPointsPerDimension + x].y = linearInterpolation(leftTop.y, rightBottom.y, static_cast<float>(y) / (numberOfPointsPerDimension - 1));
            m_points[y * numberOfPointsPerDimension + x].z = 0.f;
        }
    }

    m_transformedPoints.resize(m_points.size());
}

unsigned int Sprite3D::getPointIndexForVertexIndex(const unsigned int vertexIndex, const bool invertPointX) const
{
    const unsigned int numberOfPointsPerDimension = m_subdividedMeshDensity + 2;
    const unsigned int numberOfVerticesPerRow = numberOfPointsPerDimension * 2 - 1;

    bool isOddRow = ((vertexIndex / numberOfVerticesPerRow) % 2) == 1;
    unsigned int pointX = (vertexIndex % numberOfVerticesPerRow) / 2;
    if (isOddRow)
        pointX = numberOfPointsPerDimension - pointX - 1;
    if (invertPointX)
        pointX = numberOfPointsPerDimension - pointX - 1;
    unsigned int pointY = (vertexIndex / numberOfVerticesPerRow) + ((vertexIndex % numberOfVerticesPerRow) % 2);

    return pointY * numberOfPointsPerDimension + pointX;
}

unsigned int Sprite3D::getNumberOfVerticesNeededForCurrentSubdividedMeshDensity() const
{
    //const unsigned int numberOfPointsPerDimension = m_meshDensity + 2;
    //const unsigned int numberOfVerticesPerRow = numberOfPointsPerDimension * 2 - 1;
    //return numberOfVerticesPerRow * (numberOfPointsPerDimension - 1) + 1;
    /*
    = v * (p - 1) + 1

    v = p * 2 - 1

    = (p * 2 - 1) * (p - 1) + 1

    = (2p - 1)(p - 1) + 1

    p = m + 2

    = (2(m + 2) - 1)(m + 2 - 1) + 1

    = (2m + 4 - 1)(m + 1) + 1

    = (2m + 3)(m + 1) + 1

    = (2m� + 3m + 2m + 3) + 1

    = 2m� + 5m + 4

    = m(2m + 5) + 4
    */
    return (m_subdividedMeshDensity * 2 + 5) * m_subdividedMeshDensity + 4;
}

float Sprite3D::linearInterpolation(float from, float to, float alpha) const
{
    return from * (1.f - alpha) + to * alpha;
}

float Sprite3D::mod(float numerator, float denominator) const
{
    // avoid division by zero (if more accuracy is required, only offset the divided denominator, still use the actual denominator to multiply back as zero multiplication is fine)
    if (denominator > -0.000001f && denominator < 0.000001f)
        denominator = 0.000001f;

    return numerator - (trunc(numerator / denominator) * denominator);
}

float Sprite3D::min(float a, float b) const
{
    return (a < b) ? a : b;
}

float Sprite3D::max(float a, float b) const
{
    return (a > b) ? a : b;
}

sf::Vector2i Sprite3D::abs(const sf::Vector2i& vector) const
{
    return sf::Vector2i(std::abs(vector.x), std::abs(vector.y));
}
