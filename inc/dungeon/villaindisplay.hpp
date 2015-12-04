#pragma once

#include "scene/entity.hpp"
#include "scene/wrappers/rectangleshape.hpp"
#include "scene/wrappers/richlabel.hpp"
#include "context/villains.hpp"

#include <functional>

namespace dungeon
{
    //! Nice display of a villain stats.
    //! If not configured, will just display a "new".

    class VillainDisplay final : public scene::Entity
    {
        using baseClass = scene::Entity;

        using ClickCallback = std::function<void(VillainDisplay&)>;

    public:

        //! Constructor.
        VillainDisplay();

        //! Default destructor.
        ~VillainDisplay() = default;

        std::string _name() const { return "dungeon::VillainDisplay"; }

        //----------------//
        //! @name Control
        //! @{

        //! Get the source.
        inline const context::Villain* source() const { return m_source; }

        //! Set the source villain to display.
        void setSource(const context::Villain& villain);

        //! Set the selected mode.
        void setSelected(bool selected);

        //! @}

        //------------------//
        //! @name Callbacks
        //! @{

        //! Set the callback to call when a click occurs.
        inline void setClickCallback(ClickCallback callback) { m_clickCallback = callback; }

        //! @}

    protected:

        //----------------//
        //! @name Routine
        //! @{

        void onSizeChanges() final;

        //! @}

        //---------------//
        //! @name Events
        //! @{

        bool handleMouseButtonPressed(const sf::Mouse::Button button, const sf::Vector2f& mousePos, const sf::Vector2f& nuiPos) final;

        //! @}

        //------------//
        //! @name ICU
        //! @{

        //! Refresh the image size to the current one.
        void refreshImageSize();

        //! Refresh the general display from the source.
        void refreshFromSource();

        //! @}

    private:

        const context::Villain* m_source = nullptr; //!< The source.
        bool m_selected = false;                    //!< Is it displayed as selected?

        // Callbacks
        ClickCallback m_clickCallback = nullptr;    //!< Called whenever a click occured.

        // Decorum
        sf::RectangleShape m_background;    //!< The background.
        scene::RectangleShape m_image;      //!< The image to display.
        scene::RichLabel m_villainName;     //!< The villain name.
    };

    using VillainDisplayPtr = std::unique_ptr<VillainDisplay>;
}
