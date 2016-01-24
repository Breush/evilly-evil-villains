#pragma once

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool() noexcept
{
}

template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool() noexcept
{
    slot_pointer curr = m_currentBlock;
    while (curr != nullptr) {
        slot_pointer prev = curr->next;
        operator delete(reinterpret_cast<void*>(curr));
        curr = prev;
    }
}

//----------------------//
//----- Allocation -----//

template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer MemoryPool<T, BlockSize>::allocate()
{
    if (m_freeSlots != nullptr) {
        pointer result = reinterpret_cast<pointer>(m_freeSlots);
        m_freeSlots = m_freeSlots->next;
        return result;
    }
    else {
        if (m_currentSlot >= m_lastSlot)
            allocateBlock();
        return reinterpret_cast<pointer>(m_currentSlot++);
    }
}

template <typename T, size_t BlockSize>
inline void MemoryPool<T, BlockSize>::deallocate(pointer p)
{
    if (p != nullptr) {
        reinterpret_cast<slot_pointer>(p)->next = m_freeSlots;
        m_freeSlots = reinterpret_cast<slot_pointer>(p);
    }
}
//-------------------//
//----- Element -----//

template <typename T, size_t BlockSize>
template <class... Args>
inline typename MemoryPool<T, BlockSize>::pointer MemoryPool<T, BlockSize>::newElement(Args&&... args)
{
    pointer result = allocate();
    construct<value_type>(result, std::forward<Args>(args)...);
    return result;
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

//----------------------//
//----- Alignement -----//

template <typename T, size_t BlockSize>
inline size_t MemoryPool<T, BlockSize>::padPointer(data_pointer p, size_t align) const noexcept
{
    auto result = reinterpret_cast<uintptr_t>(p);
    return ((align - result) % align);
}

template <typename T, size_t BlockSize>
void MemoryPool<T, BlockSize>::allocateBlock()
{
    // Allocate space for the new block and store a pointer to the previous one
    auto newBlock = reinterpret_cast<data_pointer>(operator new(BlockSize));
    reinterpret_cast<slot_pointer>(newBlock)->next = m_currentBlock;
    m_currentBlock = reinterpret_cast<slot_pointer>(newBlock);

    // Pad block body to staisfy the alignment requirements for elements
    auto body = newBlock + sizeof(slot_pointer);
    auto bodyPadding = padPointer(body, alignof(slot_type));
    m_currentSlot = reinterpret_cast<slot_pointer>(body + bodyPadding);
    m_lastSlot = reinterpret_cast<slot_pointer>(newBlock + BlockSize - sizeof(slot_type) + 1);
}
