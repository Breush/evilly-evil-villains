#pragma once

#include "scene/components/lerpable.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <list>

// Forward declarations

namespace sf
{
    class Shader;
    class Event;
}

namespace config
{
    class NUIGuides;
    class WindowInfo;
}

namespace scene
{
    // Forward declarations

    class Graph;

    //! Base for every scene object.
    /*!
     *  An entity is supposed to be added to a layer of the scene::Graph.
     *  By default, it is not focusable, then does not react to keyboard/joystick
     *  as only currently focused entity gets keyboard/joystick events.
     */

    class Entity : public sf::Drawable, public sf::Transformable
    {
        friend class Layer;
        friend class Graph;

    public:

        //! Constructor.
        /*!
         *  @param isLerpable When true, the entity parameters (position, ...)
         *  will be interpolated during update(dt).
         */
        Entity(bool isLerpable = false);

        //! Destructor.
        //! The entity will detach itself if still attached to a parent.
        virtual ~Entity();

        //-------------------//
        //! @name Components
        //! @{

        //! Get the lerpable component of the entity.
        /*!
         *  Different from nullptr if entity constructor was created
         *  with lerpable option on.
         */
        sfe::Lerpable* lerpable() noexcept { return m_lerpable.get(); }

        //! @}

        //----------------------------//
        //! @name Children management
        //! @{

        //! Returns true if the given entity is already a child of this one.
        bool hasChild(const Entity& entity) const;

        //! Add an entity as a child.
        void attachChild(Entity& child);

        //! Remove an child entity.
        void detachChild(Entity& child);

        //! Remove all children entities.
        void detachChildren();

        //! @}

        //-----------------------------//
        //! @name Setters and delayers
        //! @{

        //! Delays setVisible(bool visible) to the next frame.
        void markForVisible(bool visible);

        //! Make this entity to have the keyboard focus if possible.
        void giveFocus();

        //! Set the size property.
        void setSize(const sf::Vector2f& inSize);

        //! Set the local position property.
        void setLocalPosition(const sf::Vector2f& inLocalPosition);

        //! Whether the entity is centered.
        //! This function overwrites the relativeOrigin property.
        void centerOrigin();

        //! If needed, moves the entity local coordinates so that it stays inside the borders.
        /*!
         *  If one do not want change on the horizontal (resp. vertical) axes,
         *  one just need to set width (resp. height) to negative value.
         */
        void setKeepInsideLocalRect(const sf::FloatRect& localRect);

        //! The local positions (origin taken in acount) and size.
        sf::FloatRect localBounds();

        //! Set the shader applied to the whole entity.
        void setShader(const std::string& shaderID);

        //! Set the position relative to the size of the parent.
        //! Expects inRelativePosition coordinates to be between 0.f and 1.f.
        void setRelativePosition(const sf::Vector2f& inRelativePosition);

        //! Returns the global clip area of the entity.
        inline sf::FloatRect globalClipArea() const { return m_globalClipArea; }

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! The depth of the entity in the layer.
        /*!
         *  Be aware that a child will always be drawn beyond its parent,
         *  whatever their depth are. This property is only used
         *  at the siblings level.
         */
        PARAMGSU(float, m_depth, depth, setDepth, refreshDepthOrder)

        //! The position relative to the size of the parent.
        /*!
         *  The setRelativePosition() need to be called before using this property.
         *  This overrides the localPosition property.
         */
        PARAMG(sf::Vector2f, m_relativePosition, relativePosition)

        //! The origin relative to the size of this object.
        //! Normal behavior requires values to be between 0 and 1.
        PARAMGSU(sf::Vector2f, m_relativeOrigin, relativeOrigin, setRelativeOrigin, refreshOrigin)

        //! The position of the entity relative to its parent's coordinates.
        PARAMG(sf::Vector2f, m_localPosition, localPosition)

        //! The rotation of the entity relative to its parent's rotation.
        PARAMGSU(float, m_localRotation, localRotation, setLocalRotation, refreshFromLocalRotation)

        // FIXME There is something very wrong here, find a way to update the size
        // the scale is changed?

        //! The scale of the entity relative to its parent's scale.
        /*!
         *  NOTE: There might be some incompatibility with the size property and the scale one.
         *  The design is still unclear, using both is depreciated.
         */
        PARAMGSU(sf::Vector2f, m_localScale, localScale, setLocalScale, refreshFromLocalScale)

        //! The absolute size of the entity.
        PARAMG(sf::Vector2f, m_size, size)

        //! Specifies what is drawn for the entity (also affects children).
        //! Set clipArea width or height to a negative value to draw it all.
        PARAMGSU(sf::FloatRect, m_clipArea, clipArea, setClipArea, refreshClipArea)

        //! Whether the entity and its children will be drawn, and therefore detectable.
        PARAMGS(bool, m_visible, visible, setVisible)

        //! Whether the entity will be drawn (does not affect detectability nor children).
        PARAMGS(bool, m_transparent, transparent, setTransparent)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        //! Draws the entity parts and recursively calls children draw functions.
        /*!
         *  It is virtual so that one can use external drawing functions such as
         *  sfe::AnimatedSprite handles.
         */
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

        //! Draws all parts of the entity.
        void drawParts(sf::RenderTarget& target, sf::RenderStates states) const;

        //! Called whenever the position/rotation/scale changed.
        virtual void onTransformChanges() {}

        //! Called whenever scale changed.
        virtual void onScaleChanges() {}

        //! Called whenever size changed.
        /*!
         *  Should be reimplemented in inherited classes to adapt
         *  display to the size.
         */
        virtual void onSizeChanges() {}

        //! Called whenever a child changed its size.
        virtual void onChildSizeChanges(scene::Entity& child) {}

        //! Called whenever a child has been detached.
        virtual void onChildDetached(scene::Entity& child) {}

        //! Update the entity given time-step and recursively update its children.
        /*!
         *  Calls the updateRoutine() first and update lerpable before
         *  updating its children.
         */
        void update(const sf::Time& dt);

        //! Update state at each frame given a time-step.
        /*!
         *  Should be reimplemented in child classes to manage
         *  real-time animations.
         */
        virtual void updateRoutine(const sf::Time& dt) {}

        //! Updates the state of the artificial intelligence.
        virtual void updateAI(const sf::Time& dt) {}

        //! If lerpable component enabled, update it.
        void updateLerpable(const sf::Time& dt);

        //! Check and call parent on changes if options enabled.
        void updateChanges();

        //! Refresh on window config changed.
        virtual void refreshWindow(const config::WindowInfo& cWindow);

        //! Refresh NUI appearance.
        virtual void refreshNUI(const config::NUIGuides& cNUI);

        //! Full refresh (window + NUI).
        void refresh();

        //! @}

        //----------------------------//
        //! @name Children management
        //! @{

        //! Quick access to the list of all children.
        std::list<Entity*>& children() noexcept { return m_children; }

        //! Get the first entity with the graph at given position.
        Entity* firstOver(const sf::Vector2f& position);

        //! @}

        //---------------//
        //! @name Events
        //! @{

        //! If entity is focused, this dispatches the keyboard events.
        //! Returns true if focus is kept by the entity.
        virtual bool handleKeyboardEvent(const sf::Event& event) { return false; }

        //! If entity is focused, this dispatches the joystick events.
        //! Returns true if focus is kept by the entity.
        virtual bool handleJoystickEvent(const sf::Event& event) { return false; }

        //! If entity is detectable, this is called when mouse moves over the entity.
        /*!
         *  @param mousePos The position of the mouse relative to entity coordinates.
         *  @param nuiPos   The absolute position of the mouse in the NUI layer.
         */
        virtual void handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}

        //! If entity is detectable, this is called when a mouse button is pressed over the entity.
        /*!
         *  @param button   The button pressed.
         *  @param mousePos The position of the mouse relative to entity coordinates.
         *  @param nuiPos   The absolute position of the mouse in the NUI layer.
         */
        virtual void handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}

        //! If entity is detectable, this is called when a mouse button is released over the entity.
        /*!
         *  @param button   The button released.
         *  @param mousePos The position of the mouse relative to entity coordinates.
         *  @param nuiPos   The absolute position of the mouse in the NUI layer.
         */
        virtual void handleMouseButtonReleased(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}

        //! If entity is detectable, this is called when mouse wheel moves over the entity.
        /*!
         *  @param delta    The relative delta step of the mouse wheel.
         *  @param mousePos The position of the mouse relative to entity coordinates.
         *  @param nuiPos   The absolute position of the mouse in the NUI layer.
         */
        virtual void handleMouseWheelMoved(const int delta, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) {}

        //! If entity is detectable, this is called when mouse leaves the entity.
        virtual void handleMouseLeft() {}

        //! @}

        //--------------//
        //! @name Graph
        //! @{

        //! Set the current graph which stores this entity.
        void setGraph(Graph* graph);

        //! Set the parent of the entity.
        void setParent(Entity* inParent);

        //! Recursive call to get the root of the layer containing the entity.
        Entity* root();

        //! Returns the previous sibling. If none, returns nullptr.
        Entity* closestPreviousSibling();

        //! Returns the next sibling. If none, returns nullptr.
        Entity* closestNextSibling();

        //! Returns the right-most descendant of the tree.
        Entity* lastDescendant();

        //! @}

        //--------------//
        //! @name Parts
        //! @{

        //! An entity is just a bunch of drawable parts.
        struct Part {
            sf::Drawable*   drawable;       //!< Drawable part.
            sf::Shader*     shader;         //!< Shader to apply to the part.
            bool            clipping;       //!< Whether the part needs clipping.
            sf::FloatRect   clippingRect;   //!< If clipping, the clipping rectangle.
        };

        //! Add a drawable as a part.
        void addPart(sf::Drawable* drawable);

        //! Set shader of the part.
        void setPartShader(sf::Drawable* drawable, const std::string& shader);

        //! Reset this part shader to nullptr.
        void resetPartShader(sf::Drawable* drawable);

        //! Reset all parts shader to nullptr.
        void resetPartsShader();

        //! Set clippingRect of the part.
        /*!
         *  Once called, clipping cannot be deactivated without removing
         *  the part from the list.
         */
        void setPartClippingRect(sf::Drawable* drawable, const sf::FloatRect& clippingRect);

        //! Remove a given part from the list.
        void removePart(sf::Drawable* drawable);

        //! Remove all parts from the list.
        void clearParts();

        //! @}

        //---------------------------//
        //! @name Refresh on changes
        //! @{

        //! Refresh the origin of the entity.
        void refreshOrigin();

        //! Refresh the position relative to the size of the parent.
        void refreshRelativePosition();

        //! Refresh all absolute parameters.
        void refreshFromLocal();

        //! Refresh absolute position.
        void refreshFromLocalPosition();

        //! Refresh absolute rotation.
        void refreshFromLocalRotation();

        //! Refresh absolute scale.
        void refreshFromLocalScale();

        //! Refresh focusRect position, if focus is not owned.
        void refreshFocusRect();

        //! Sort parent children according to depth.
        void refreshDepthOrder();

        //! Sort own children according to their depth.
        void refreshChildrenOrder();

        //! Refresh the relative position of children.
        void refreshChildrenRelativePosition();

        //! Refresh the clip area and update children.
        void refreshClipArea();

        //! Moves the entity to keep it between m_insideLocalRect coordinates.
        //! @return true if m_localPosition has been modified.
        // TODO For this to be fully effective, we need to override setOrigin()
        // so that we can do a refreshKeepInsideRect() on change
        // which is almost done in setRelativeOrigin...
        bool refreshKeepInsideLocalRect();

        //! @}

        //------------------------//
        //! @name Focusing system
        //! @{

        //! Tells the entity that it is currently focused.
        //! This is used to call the graph back when focusRect changes.
        void setFocused(bool inFocused);

        //! Set the focusRect if focusOwned property is on.
        void setFocusRect(const sf::FloatRect& inFocusRect);

        //! Graph-wise function to get the previous focusable entity.
        //! Will not loop and does return nullptr if not found.
        Entity* previousFocusable();

        //! Graph-wise function to get the next focusable entity.
        //! Will not loop and does return nullptr if not found.
        Entity* nextFocusable();

        //! Utility function to get the first focusable child.
        //! Returns nullptr if not found.
        Entity* nextFocusableChild();

        //! Whether the entity is currently focused (state managed by the Graph).
        inline bool focused() const { return m_focused; }

        //! @}

        //-----------------------------//
        //! @name Protected properties
        //! @{

        //! Pointer to the graph.
        PARAMG(Graph*, m_graph, graph)

        //! Pointer to the parent if in graph.
        PARAMG(Entity*, m_parent, parent)

        //! Whether the mouse events affects the entity.
        PARAMGS(bool, m_detectable, detectable, setDetectable)

        //! Whether the entity is focusable, and can get events from keyboard/joystick.
        /*!
         *  This property only affects entities pushed into the
         *  NUI layer of the scene::Graph.
         */
        PARAMGS(bool, m_focusable, focusable, setFocusable)

        //! Whether the entity manage itself the focusRect.
        PARAMGS(bool, m_focusOwned, focusOwned, setFocusOwned)

        //! The focusRect is the highlighted part of the entity when focused.
        PARAMG(sf::FloatRect, m_focusRect, focusRect)

        //! @}

    private:

        //! The children, as an entity is also a node in a graph.
        std::list<Entity*> m_children;

        //! The parts, as an entity is something drawn.
        std::vector<Part> m_parts;

        //! The lerpable component.
        std::unique_ptr<sfe::Lerpable> m_lerpable = nullptr;

        //! The shader used to draw the entity.
        sf::Shader* m_shader = nullptr;

        //! Whether the entity is currently focused.
        bool m_focused = false;

        //! Whether the size has changed since last update.
        bool m_sizeChanges = true;

        //! Whether the local transformations has changed since last update.
        bool m_localChanges = true;

        //! Whether the position of the entity should be computed relatively.
        bool m_relativePositioning = false;

        //! Whether the entity is marked for a delayed setVisible() call.
        bool m_markedForVisible = false;

        //! The target status to visible, if mark is on.
        bool m_visibleMark;

        //! The visual part of the entity, taking parent clip area in account.
        sf::FloatRect m_globalClipArea = {0.f, 0.f, -1.f, -1.f};

        //! The entity will be kept between this limits if width/height is not negative.
        sf::FloatRect m_insideLocalRect = {0.f, 0.f, -1.f, -1.f};

        //! Whether or not this entity has a valid globalClipArea.
        bool m_globalClipping = false;
    };
}
