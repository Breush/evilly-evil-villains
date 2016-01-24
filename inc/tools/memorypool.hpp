#pragma once

#include <climits>
#include <cstddef>

//! A memory pool.

template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
    using value_type =      T;
    using pointer =         T*;
    using reference =       T&;
    using const_pointer =   const T*;
    using const_reference = const T&;

    using data_pointer = char*;

public:

    //! Constructor.
    MemoryPool() noexcept;

    //! Destructor.
    ~MemoryPool() noexcept;

    //-------------------//
    //! @name Allocation
    //! @{

    //! Allocate one object at a time. Won't call constructor.
    pointer allocate();

    //! Deallocate an allocated object. Won't call destructor.
    void deallocate(pointer p);

    //! @}

    //----------------//
    //! @name Element
    //! @{

    //! Construct an element, with all its arguments.
    template <class... Args> pointer newElement(Args&&... args);

    //! Delete an element.
    void deleteElement(pointer p);

    //! Construct a different element, with all its arguments.
    template <class U, class... Args> void construct(U* p, Args&&... args);

    //! Delete a different element.
    template <class U> void destroy(U* p);

    //! @}

protected:

    //---------------------------//
    //! @name Alignement control
    //! @{

    //! Align the pointer.
    size_t padPointer(data_pointer p, size_t align) const noexcept;

    //! Allocate the whole block.
    void allocateBlock();

    //! @}

    //! All available space.
    union Slot_
    {
        value_type element; //!< An element.
        Slot_* next;        //!< Next free slot.
    };

    using slot_type    = Slot_ ;
    using slot_pointer = Slot_*;

private:

    slot_pointer m_currentBlock = nullptr;
    slot_pointer m_currentSlot  = nullptr;
    slot_pointer m_lastSlot     = nullptr;
    slot_pointer m_freeSlots    = nullptr;
};

#include "tools/memorypool.inl"

