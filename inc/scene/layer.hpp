#pragma once

#include "scene/entity.hpp"

// Forward declarations

enum class ViewID : uint8;

namespace sf
{
    class View;
}

// Enums

//! All layer identifiers.
namespace LayerID
{
    enum type {
        DUNGEON_DESIGN  = 0, //!< For dungeon design mode
        NUI             = 1, //!< Keep it at the end, will be over everything
        COUNT           = 2, //!< Numbers of layers, keep it last
    };
}

namespace scene
{
    //! A layer within the Graph.
    /*!
     *  A layer has a fixed size and acts on a view.
     *  If set manipulable, it reacts to grabbing and zooming.
     *  It is also the container for the initial node in the Graph layer.
     */

    class Layer final
    {
    public:

        //! Constructor.
        Layer();

        //----------------//
        //! @name Getters
        //! @{

        //! The first entity in the layer.
        inline Entity& root() { return m_root; }

        //! The first (const) entity in the layer.
        inline const Entity& root() const { return m_root; }

        //! The view as previously set.
        inline const sf::View& view() const { return *m_view; }

        //! @}

        //--------------------------//
        //! @name View manipulation
        //! @{

        //! Set the view to be used.
        void setView(ViewID viewID);

        //! Relative zoom to current zoom.
        void zoom(float zoomFactor);

        //! Relative move to current position.
        void move(const sf::Vector2f& offset);

        //! @}

        //-----------------//
        //! @name Grabbing
        //! @{

        //! To be called when the grabbing of the layer starts.
        void startGrabbing(const sf::Vector2f& position);

        //! To be called when it's currently grabbing and need to move.
        void moveGrabbing(const sf::Vector2f& position);

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether the layer needs to control the view.
        PARAMGS(bool, m_manipulable, manipulable, setManipulable)

        //! The start and the size of the region that can be displayed.
        PARAMGSU(sf::FloatRect, m_displayRect, displayRect, setDisplayRect, updateZoomInfo)

        //! The minimal zoom, relative to view reference size.
        PARAMGS(float, m_minZoom, minZoom, setMinZoom)

        //! The maximal zoom, relative to view reference size.
        PARAMGS(float, m_maxZoom, maxZoom, setMaxZoom)

        //! @}

    protected:

        //---------------//
        //! @name Limits
        //! @{

        //! Move the view so that it is inside the displayRect.
        void adaptViewPosition();

        //! Zoom the view so that it is inside the minimum and maximum zoom.
        void adaptViewZoom();

        //! @}

        //-------------------------------//
        //! @name Internal state updates
        //! @{

        //! Updates minimum and maximum zoom and size and center the view.
        void updateZoomInfo();

        //! @}

    private:

        //! The entity, as a layer holds the first node.
        Entity m_root;

        //! The view, as the layer controls its the displayed region.
        sf::View* m_view = nullptr;

        //! The grabbing position reference.
        sf::Vector2f m_grabbingPosition;

        float m_relZoom = 1.f;  //!< The current zoom, relative to view reference size.

        sf::Vector2f m_minSize; //!< The minimal size allowed.
        sf::Vector2f m_maxSize; //!< The maximal size allowed.
    };
}
