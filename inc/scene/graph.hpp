#pragma once

#include "scene/grabbable.hpp"
#include "scene/entity.hpp"
#include "scene/scene.hpp"
#include "scene/layer.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

namespace scene
{
    //! Scene graph to manage drawing, focus and events of entities.
    //! It stores the NUI layer and the scene (multiple layers).

    class Graph final : public sf::Drawable
    {
        friend class Entity;

    public:

        //! Constructor.
        Graph();

        //! Default destructor.
        ~Graph() = default;

        //----------------//
        //! @name Routine
        //! @{

        //! Draw the graph recursively.
        void draw(sf::RenderTarget& target, sf::RenderStates states) const final;

        //! Update the graph recursively.
        void update(const sf::Time& dt, const float factor);

        //! Handle event recursively.
        void handleEvent(const sf::Event& event);

        //! Reset views to current screen status.
        void refreshWindow(const config::WindowInfo& cWindow);

        //! Refresh NUI appearance.
        void refreshNUI(const config::NUIGuides& cNUI);

        //! @}

        //----------------------//
        //! @name Access layers
        //! @{

        //! The NUI layer.
        inline Layer& nuiLayer() { return m_nuiLayer; }

        //! The scene (multiple layers).
        inline Scene& scene() { return m_scene; }

        //! @}

        //-----------------//
        //! @name Grabbing
        //! @{

        //! Set an object to be the current grabbable.
        void setGrabbable(std::unique_ptr<Grabbable> grabbable);

        //! Remove the currently grabbed object.
        void removeGrabbable();

        //! Access to the grabbed object.
        Grabbable* grabbable() { return m_grabbable.get(); }

        //! @}

    protected:

        //---------------//
        //! @name Events
        //! @{

        //! Broadcast an event to all global handlers.
        void broadcastGlobalEvent(const sf::Event& event);

        //! Called when the event is a mouse wheel pressed and if no entity used it.
        void handleMouseWheelPressedEvent(const sf::Event& event);

        //! Called when the event is a mouse wheel released and if no entity used it.
        void handleMouseWheelReleasedEvent(const sf::Event& event);

        //! Called when the event is a mouse wheel moved and if no entity used it.
        void handleMouseWheelMovedEvent(const sf::Event& event);

        //! Called when the event is a mouse moved, before any entity uses it,
        //! have to return true to keep event.
        bool handleMouseMovedEvent(const sf::Event& event);

        //! @}

        //-------------------------//
        //! @name Entity callbacks
        //! @{

        //! Called whenever an entity is detached from this graph.
        void detachedEntity(Entity* entity);

        //! Called whenever an entity is attached to this graph.
        void attachedEntity(Entity* entity);

        //! @}

        //------------------------//
        //! @name Focusing system
        //! @{

        //! Set the currently focused entity.
        void setFocusedEntity(Entity* focusedEntity);

        //! Let focused entity managed the event.
        void focusHandleEvent(const sf::Event& event);

        //! @}

        //------------------------//
        //! @name Mouse detection
        //! @{

        //! Manage the mouse event, return the first detectable entity below the mouse position.
        Entity* handleMouseEvent(const sf::Event& event, bool& entityKeptEvent);

        //! Debug interface drawing detection box of hovered entity.
        void drawMouseDetector(sf::RenderTarget& target, sf::RenderStates states) const;

        //! Set the hovered entity.
        void setHoveredEntity(Entity* hoveredEntity);

        //! Get the position in NUI from mouse position.
        //! Let default value in order to automatically evaluate mouse position.
        sf::Vector2f nuiPosition(sf::Vector2i mousePosition = {-1, -1});

        //! @}

        //-----------------//
        //! @name Grabbing
        //! @{

        //! Let the grabbable object manage the event.
        void grabbableHandleMouseEvent(const sf::Event& event);

        //! @}

        //-------------------------//
        //! @name Entity detection
        //! @{

        //! Find the front-most detectable entity below the mouse position.
        //! \param viewPos will be set to the position in the entity's layer's view.
        Entity* entityFromPosition(const sf::Vector2i& mousePos, sf::Vector2f& viewPos);

        //! Get the view associated to specified layer's root.
        const sf::View& viewFromLayerRoot(const Entity* root) const;

        //! @}

    private:

        // Layers
        Scene m_scene;      //!< The main scene.
        Layer m_nuiLayer;   //!< The NUI layer.

        // Focusing system
        Entity* m_focusedEntity = nullptr;  //!< The currently focused entity.

        // Mouse detection
        Entity* m_hoveredEntity = nullptr;  //!< The currently hovered entity.

        // Global events
        std::list<Entity*> m_globalHandlers;    //!< All the registered entities to global events.

        // Grabbing
        bool m_grabbing = false;                            //!< Whether the grabbing scene is active or not.
        sf::Vector2i m_grabbingPosition;                    //!< Start of grabbing mouse position.
        std::unique_ptr<Grabbable> m_grabbable = nullptr;   //!< The grabbed object.
    };
}
