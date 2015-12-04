#pragma once

#include "tools/int.hpp"
#include "context/wallet.hpp"

#include <string>
#include <vector>
#include <ctime>

#include <SFML/System/NonCopyable.hpp>

namespace context
{
    //! Defines villain attributes.

    class Villain
    {
    public:

        //--------------------//
        //! @name Interaction
        //! @{

        //! Find how many worlds are associated with the specified villain.
        uint worldsCount() const;

        //! @}

        uint index;         //!< The index of the villain in the villains vector.
        std::wstring name;  //!< The name of the villain.
        Wallet doshWallet;  //!< Manage the dosh possessed by the villain.
        bool man = true;   //!< Is the villain using man asset?
    };

    //! A XML loading class to get the villains data.
    /*!
     *  This class allow you to list a villain
     *  coming from the default file saves/villains.xml.
     */

    class Villains final : sf::NonCopyable
    {
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

        //! Find and delete the specified villain and all its associated worlds.
        void remove(uint index);

        //! Find and delete all worlds that are associated with the specified villain.
        void removeWorlds(uint index);

        //! @}

        //----------------//
        //! @name Getters
        //! @{

        //! Get the vector of all villains for read.
        inline const std::vector<Villain>& get() const { return m_villains; }

        //! Get the specific villain that matches the name.
        //! Returns nullptr if not found.
        Villain* getFromVillainName(const std::wstring& villainName);

        //! Get the specific villain that owns the world in the specified folder.
        //! Returns nullptr if not found.
        Villain* getFromWorldFolder(const std::wstring& folder);

        //! Get a specific villain for read.
        inline const Villain& get(uint index) const { return m_villains[index]; }

        //! @}

    private:

        std::vector<Villain> m_villains;    //!< The vector of all villains.
    };

    //! The static member from villains context.
    extern Villains villains;
}

