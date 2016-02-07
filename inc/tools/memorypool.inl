#pragma once

#include <utility>
#include <cstdint>

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool()
{
    // Allocate the whole block
    m_blocks = static_cast<pointer>(malloc(sizeof(value_type) * BlockSize));
    for (uint i = 0u; i < BlockSize; ++i)
        m_freeBlocksIndexes.emplace_back(i);
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool() noexcept
{
    // Everything is dropped to oblivion (no destructor called)
    free(m_blocks);
}

//----------------------//
//----- Allocation -----//

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer MemoryPool<T, BlockSize>::allocate()
{
    auto p = m_blocks + m_freeBlocksIndexes.front();
    m_freeBlocksIndexes.pop_front();
    return p;
}

template <typename T, size_t BlockSize>
inline void MemoryPool<T, BlockSize>::deallocate(pointer p)
{
    uint index = p - m_blocks;
    m_freeBlocksIndexes.emplace_front(index);
}
//-------------------//
//----- Element -----//

template <typename T, size_t BlockSize>
template <class... Args>
inline typename MemoryPool<T, BlockSize>::pointer MemoryPool<T, BlockSize>::newElement(Args&&... args)
{
    auto p = allocate();
    new (p) value_type(std::forward<Args>(args)...);
    return p;
}

template <typename T, size_t BlockSize>
inline void MemoryPool<T, BlockSize>::deleteElement(pointer p)
{
    if (p != nullptr) {
        p->~value_type();
        deallocate(p);
    }
}

template <typename T, size_t BlockSize>
template <class U, class... Args>
inline void MemoryPool<T, BlockSize>::construct(U* p, Args&&... args)
{
    new (p) U (std::forward<Args>(args)...);
}

template <typename T, size_t BlockSize>
template <class U>
inline void MemoryPool<T, BlockSize>::destroy(U* p)
{
    p->~U();
}
