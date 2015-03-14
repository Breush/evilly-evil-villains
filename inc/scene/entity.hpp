#pragma once

#include "sfe/lerpable.hpp"
#include "tools/param.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Window/Mouse.hpp>
#include <memory>
#include <list>

// Forward declarations

enum class ShaderID : uint8;

namespace sf
{
    class Shader;
    class Event;
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

        //! Default destructor.
        virtual ~Entity() = default;

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

        //! Add an entity as a child.
        void attachChild(Entity& child);

        //! Remove an child entity.
        void detachChild(Entity& child);

        //! @}

        //-----------------------------//
        //! @name Setters and delayers
        //! @{

        //! Delays setVisible(bool visible) to the next frame.
        void markForVisible(bool visible);

        //! Set the size property.
        void setSize(const sf::Vector2f& inSize);

        //! Set the shader applied to the whole entity.
        /*!
         *  Use ShaderID::NONE to remove current shader.
         */
        void setShader(ShaderID shaderID);

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

        //! The position of the entity relative to its parent's coordinates.
        PARAMGSU(sf::Vector2f, m_localPosition, localPosition, setLocalPosition, refreshFromLocalPosition)

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

        //! Whether the origin of the entity needs to be recentered everytime the size changes.
        PARAMGSU(bool, m_centered, centered, setCentered, refreshCentering)

        //! Whether the entity will be drawn, and therefore detectable.
        PARAMGS(bool, m_visible, visible, setVisible)

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

        //! Called whenever size or a child changed if callParent is on.
        /*!
         *  Should be reimplemented in inherited classes to adapt
         *  display to the size.
         */
        virtual void update() {}

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

        //! If lerpable component enabled, update it.
        void updateLerpable(const sf::Time& dt);

        //! Check and call parent on changes if options enabled.
        void updateChanges();

        //! @}

        //----------------------------//
        //! @name Children management
        //! @{

        std::list<Entity*>& children() noexcept { return m_children; }
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
        void setPartShader(sf::Drawable* drawable, ShaderID shader);

        //! Reset all parts to ShaderID::NONE.
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

        //! Refresh origin if option centered is on.
        void refreshCentering();

        //! Sort parent children according to depth.
        void refreshDepthOrder();

        //! Sort own children according to their depth.
        void refreshChildrenOrder();

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

        //! Whether to call parent when size changes.
        bool m_callParentOnSizeChanges = true;

        //! Whether to call parent when local transformations changes.
        bool m_callParentOnLocalChanges = false;

        //! Whether the entity is marked for a delayed setVisible() call.
        bool m_markedForVisible = false;

        //! The target status to visible, if mark is on.
        bool m_visibleMark;
    };
}
