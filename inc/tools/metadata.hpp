#pragma once

#include "tools/int.hpp"
#include "tools/string.hpp"

union MetaData_t
{
    uint64 raw;

    int8 as_int8;
    int16 as_int16;
    int32 as_int32;
    int64 as_int64;

    uint8 as_uint8;
    uint16 as_uint16;
    uint32 as_uint32;
    uint64 as_uint64;
};

class MetaData
{
public:

    inline MetaData_t& setType(std::wstring type) { m_type = std::move(type); return m_data; }
    inline const std::wstring& type() const { return m_type; }

    inline int8& operator= (const int8& rhs)  { return m_data.as_int8  = rhs; }
    inline int16& operator=(const int16& rhs) { return m_data.as_int16 = rhs; }
    inline int32& operator=(const int32& rhs) { return m_data.as_int32 = rhs; }
    inline int64& operator=(const int64& rhs) { return m_data.as_int64 = rhs; }

    inline uint8& operator= (const uint8& rhs)  { return m_data.as_uint8  = rhs; }
    inline uint16& operator=(const uint16& rhs) { return m_data.as_uint16 = rhs; }
    inline uint32& operator=(const uint32& rhs) { return m_data.as_uint32 = rhs; }
    inline uint64& operator=(const uint64& rhs) { return m_data.as_uint64 = rhs; }

    inline const int8&  as_int8()  const { return m_data.as_int8; }
    inline const int16& as_int16() const { return m_data.as_int16; }
    inline const int32& as_int32() const { return m_data.as_int32; }
    inline const int64& as_int64() const { return m_data.as_int64; }

    inline const uint8&  as_uint8()  const { return m_data.as_uint8; }
    inline const uint16& as_uint16() const { return m_data.as_uint16; }
    inline const uint32& as_uint32() const { return m_data.as_uint32; }
    inline const uint64& as_uint64() const { return m_data.as_uint64; }

    inline int8&  as_int8()  { return m_data.as_int8; }
    inline int16& as_int16() { return m_data.as_int16; }
    inline int32& as_int32() { return m_data.as_int32; }
    inline int64& as_int64() { return m_data.as_int64; }

    inline uint8&  as_uint8()  { return m_data.as_uint8; }
    inline uint16& as_uint16() { return m_data.as_uint16; }
    inline uint32& as_uint32() { return m_data.as_uint32; }
    inline uint64& as_uint64() { return m_data.as_uint64; }

    inline void init_int8 (int8  value) { m_type = L"int8";  m_data.as_int8  = value; }
    inline void init_int16(int16 value) { m_type = L"int16"; m_data.as_int16 = value; }
    inline void init_int32(int32 value) { m_type = L"int32"; m_data.as_int32 = value; }
    inline void init_int64(int64 value) { m_type = L"int64"; m_data.as_int64 = value; }

    inline void init_uint8 (uint8 value)  { m_type = L"uint8";  m_data.as_uint8  = value; }
    inline void init_uint16(uint16 value) { m_type = L"uint16"; m_data.as_uint16 = value; }
    inline void init_uint32(uint32 value) { m_type = L"uint32"; m_data.as_uint32 = value; }
    inline void init_uint64(uint64 value) { m_type = L"uint64"; m_data.as_uint64 = value; }

private:

    std::wstring m_type;
    MetaData_t m_data;
};

