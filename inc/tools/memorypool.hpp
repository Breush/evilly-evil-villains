#pragma once

#include "tools/int.hpp"

#include <climits>
#include <cstddef>

#include <array>
#include <deque>

//! A memory pool.

template <typename T, size_t BlockSize = 4096u>
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
    MemoryPool();

    //! Destructor, won't call per element destructors.
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

    //! Construct a different element type, with all its arguments.
    template <class U, class... Args> void construct(U* p, Args&&... args);

    //! Delete a different element type.
    template <class U> void destroy(U* p);

    //! @}

private:

    pointer m_blocks;                       //!< The memory pool blocks.
    std::deque<uint> m_freeBlocksIndexes;   //!< The indexes of all free blocks.
};

#include "tools/memorypool.inl"

