#pragma once

#include "scene/entity.hpp"
#include "tools/int.hpp"

#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <map>

namespace nui
{
    //! A sprite with certain parts clickable.
    /*!
     *  Reacts are the rectangle sub-parts of an image,
     *  they are link to key string.
     */

    class ReactImage final : public scene::Entity
    {
        using baseClass = scene::Entity;

    public:

        //! Callback type for functions called when a react is clicked.
        using Callback = std::function<void()>;

    public:

        //! Constructor.
        ReactImage();

        //! Default destructor.
        ~ReactImage() = default;

        //--------------//
        //! @name Control
        //! @{

        //! Load texture and reacts information from a XML file.
        void loadFromFile(const std::string& file);

        //! @}

        //----------------//
        //! @name Texture
        //! @{

        //! Set the texture used.
        void setTexture(const std::string& textureId);

        //! @}

        //-------------------------//
        //! @name React management
        //! @{

        //! True if active react changed since last update.
        bool activeReactChanged() const { return m_activeReactChanged; }

        //! Add a new react to the list.
        void addReact(std::wstring key, sf::IntRect rect, const Callback callback = nullptr);

        //! Sets the react callback a posteriori.
        void setReactCallback(const std::wstring& key, const Callback callback);

        //! Select a react to be active.
        void setActiveReact(const std::wstring& key);

        //! Get the key of the currently active react.
        inline const std::wstring* activeReactKey() const { return (m_activeReact != nullptr)? &m_activeReact->key : nullptr; }

        //! @}

        //--------------------------//
        //! @name Public properties
        //! @{

        //! Whether or not to deselect react when mouse is over none.
        PARAMGS(bool, m_mouseLeftDeselect, mouseLeftDeselect, setMouseLeftDeselect)

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void updateRoutine(const sf::Time& dt) final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        bool handleMouseMoved(const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;
        void handleMouseLeft() final;

        //! @}

        //-------------------------------------//
        //! @name Selection of draw rectangles
        //! @{

        // Affect visible rectangles to sprite
        void selectImageRect();
        void selectActiveRect();

        //! @}

        //--------------------------------//
        //! @name Internal changes update
        //! @{

        //! Refresh the internal buffer to current state.
        void refreshBuffer();

        //! @}

        //! All react information.
        struct ReactInfo
        {
            std::wstring key;               //!< The key for this react.
            sf::IntRect rect;               //!< The rectangle sub-part of the image.
            Callback callback = nullptr;    //!< The callback to call when this part is clicked.
        };

    private:

        // Image
        sf::RenderTexture m_buffer;    //!< The internal buffer.
        sf::Sprite m_image;            //!< The original texture.
        sf::Sprite m_sprite;           //!< Sprite displaying the final texture from the buffer.
        sf::FloatRect m_imageRect;     //!< The sub-part to draw to buffer.

        // React
        std::vector<ReactInfo> m_reacts;            //!< All the reacts.
        const ReactInfo* m_activeReact = nullptr;   //!< The active react.
        bool m_activeReactChanged = false;          //!< True if active react changed since last update.
    };
}

