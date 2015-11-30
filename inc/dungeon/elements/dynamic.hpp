#pragma once

#include "dungeon/elements/dynamicelement.hpp"

namespace dungeon
{
    //! A generic dynamic.
    /*! This inherits from DynamicElement and is a generic representation
     *  of what are called dynamics inside the dungeon.
     */

    class Dynamic final : public DynamicElement
    {
        using baseClass = DynamicElement;

        std::string detectKey() const final { return "dynamic"; }
        std::string _name() const final { return "dungeon::Dynamic"; }

    public:

        //! Constructor.
        Dynamic(Inter& inter);

        //! Default destructor.
        virtual ~Dynamic() = default;

        //---------------------//
        //! @name Element data
        //! @{

        //! Rebinds the element data.
        void bindElementData(ElementData& edata);

        //! @}

    protected:

        //----------------//
        //! @name Lua API
        //! @{

        //! Destroy this entity.
        void lua_dynamicRemoveSelf();

        //! @}

    private:

        //! The id of the dynamic.
        std::wstring m_elementID;
    };
}
