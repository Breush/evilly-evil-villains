#pragma once

#include "tools/param.hpp"

#include <SFML/System/Time.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include <memory> // shared_ptr
#include <string>
#include <list>
#include <unordered_map>

namespace scene
{
    // Forward declarations

    class Entity;
    class Graph;
    class Layer;

    //! A bunch of manipulable layers representing the game.
    /*!
     *
     */

    class Scene final : public sf::Drawable
    {
    public:

        //! Deleted default constructor.
        Scene() = delete;

        //! Constructor with original graph.
        Scene(Graph* graph);

        //----------------//
        //! @name Routine
        //! @{

        //! Recursively updates all layers.
        void update(const sf::Time& dt);

        //! Recursively draw all layers.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Reset the layers' views to current screen status.
        void refreshDisplay();

        //! @}

        //--------------------------//
        //! @name View manipulation
        //! @{

        //! Sets the viewport.
        //! If not used, the default is resolution-wide.
        void setViewport(const sf::FloatRect& viewport);

        //! Center the view to the specified relative coordinates.
        void centerRelative(const sf::Vector2f& relCoords);

        //! To be called when the grabbing of the scene starts.
        void startGrabbing(const sf::Vector2i& mousePos);

        //! To be called when it's currently grabbing and need to move.
        void moveGrabbing(const sf::Vector2i& mousePos);

        //! Ask for a zoom to a specific point.
        void zoom(const sf::Vector2i& mousePos, float zoomFactor);

        //! Recursively checks the first entity at specific position in all layers.
        Entity* entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos);

        //! @}

        //-------------------------//
        //! @name Layer management
        //! @{

        //! Add a layer to the scene.
        Layer& addLayer(const std::string& key, float depth);

        //! Retrieve a layer given its key.
        inline Layer& layer(const std::string& key) { return *m_layersMap[key]; }

        //! Get all the layers of the scene.
        inline const std::list<std::shared_ptr<Layer>>& layers() const { return m_layers; }

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! The minimal zoom, relative to view reference size.
        PARAMGSU(float, m_minZoom, minZoom, setMinZoom, updateReferenceMinMax)

        //! The maximal zoom, relative to view reference size.
        PARAMGSU(float, m_maxZoom, maxZoom, setMaxZoom, updateReferenceMinMax)

        //! The size of the scene.
        PARAMGSU(sf::Vector2f, m_size, size, setSize, updateReferenceMinMax)

        //! @}

    protected:

        //-------------------------------//
        //! @name Internal state updates
        //! @{

        //! Updates the zoom factors of all layers given the reference one.
        void updateLayersZoom();

        //! Updates the position of all layers given the reference one.
        void updateLayersPosition();

        //! Updates minimum and maximum zoom and size and center the view.
        void updateReferenceMinMax();

        //! @}

        //---------------//
        //! @name Limits
        //! @{

        //! Move the view so that it is inside the displayRect.
        void adaptViewPosition();

        //! Zoom the view so that it is inside the minimum and maximum zoom.
        void adaptViewZoom();

        //! @}

    private:

        //! Original graph.
        Graph* m_graph;

        //! The list of all layers.
        /*!
         *  Invariant: for all layer in m_layers, there is a
         *  std::string key in m_layersMap such that m_layersMap[key] = layer.
         */
        std::list<std::shared_ptr<Layer>> m_layers;

        //! The map holding keys to the layers, used for quick access.
        std::unordered_map<std::string, std::shared_ptr<Layer>> m_layersMap;

        //! The grabbing position reference.
        sf::Vector2f m_grabbingPosition;

        //! An emulated view supposed to be a layer at depth 1.
        //! It is never used to display anything, just to facilitate computations.
        sf::View m_refView;

        float m_maxDepth = 0.f; //!< The current max depth within all layers.
        float m_relZoom = 1.f;  //!< The current zoom, relative to view reference size.

        sf::Vector2f m_minSize; //!< The minimal size allowed.
        sf::Vector2f m_maxSize; //!< The maximal size allowed.

        sf::Vector2f m_relativeCenter;  //!< The relative center.

        // Viewports
        bool m_ownViewport = false; //!< Whether or not we use a provided viewport.
        sf::FloatRect m_viewport;   //!< The provided viewport if any.
    };
}
