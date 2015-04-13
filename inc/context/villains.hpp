#pragma once

#include "tools/int.hpp"

#include <string>
#include <vector>
#include <ctime>

#include <SFML/System/NonCopyable.hpp>

namespace context
{
    //! A XML loading class to get the villains data.
    /*!
     *  This class allow you to list a villain
     *  coming from the default file saves/villains.xml.
     */

    class Villains final : sf::NonCopyable
    {
    public:

        //! Defines villain attributes.
        struct Villain
        {
            uint index;                 //!< The index of the villain in the villains vector.
            std::wstring name;          //!< The name of the villain.
        };

    public:

        //! Default constructor.
        Villains() = default;

        //! Default destructor.
        ~Villains() = default;

        //------------------------//
        //! @name File management
        //! @{

        //! Reload all the villains into memory.
        void load();

        //! Save all the villains to the default file.
        void save();

        //! @}

        //--------------------//
        //! @name Interaction
        //! @{

        //! Add a villain in memory (nothing saved yet).
        //! @return The index of the new villain.
        uint add(std::wstring name);

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Get the vector of all villains for read.
        inline const std::vector<Villain>& get() const { return m_villains; }

        //! Get a specific villain for read.
        inline const Villain& get(uint index) const { return m_villains[index]; }

        //! @}

    private:

        std::vector<Villain> m_villains;    //!< The vector of all villains.
    };

    //! The static member from villains context.
    extern Villains villains;
}

