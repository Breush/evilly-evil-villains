#pragma once

#include "tools/int.hpp"
#include "tools/string.hpp"

template<typename T>
struct Vec2
{
    T x;
    T y;
};

union MetaData_t
{
    bool as_bool;

    int8 as_int8;
    int16 as_int16;
    int32 as_int32;
    int64 as_int64;

    uint8 as_uint8;
    uint16 as_uint16;
    uint32 as_uint32;
    uint64 as_uint64;

    float as_float;
    double as_double;

    Vec2<uint8> as_v2uint8;
    Vec2<float> as_v2float;
};

//! A class that can handle and remember a variable of various types.

class MetaData final
{
public:

    //! Default constructor.
    MetaData() = default;

    inline MetaData_t& setType(std::wstring type) { m_type = std::move(type); return m_data; }
    inline const std::wstring& type() const { return m_type; }

    inline bool& operator=(const bool& rhs) { massert(m_type == L"bool", "Writting incompatible value to MetaData."); return m_data.as_bool = rhs; }

    inline int8& operator= (const int8& rhs)  { massert(m_type == L"int8",  "Writting incompatible value to MetaData."); return m_data.as_int8  = rhs; }
    inline int16& operator=(const int16& rhs) { massert(m_type == L"int16", "Writting incompatible value to MetaData."); return m_data.as_int16 = rhs; }
    inline int32& operator=(const int32& rhs) { massert(m_type == L"int32", "Writting incompatible value to MetaData."); return m_data.as_int32 = rhs; }
    inline int64& operator=(const int64& rhs) { massert(m_type == L"int64",  "Writting incompatible value to MetaData."); return m_data.as_int64 = rhs; }

    inline uint8& operator= (const uint8& rhs)  { massert(m_type == L"uint8",  "Writting incompatible value to MetaData."); return m_data.as_uint8  = rhs; }
    inline uint16& operator=(const uint16& rhs) { massert(m_type == L"uint16", "Writting incompatible value to MetaData."); return m_data.as_uint16 = rhs; }
    inline uint32& operator=(const uint32& rhs) { massert(m_type == L"uint32", "Writting incompatible value to MetaData."); return m_data.as_uint32 = rhs; }
    inline uint64& operator=(const uint64& rhs) { massert(m_type == L"uint64", "Writting incompatible value to MetaData."); return m_data.as_uint64 = rhs; }

    inline float&  operator=(const float& rhs)  { massert(m_type == L"float",  "Writting incompatible value to MetaData."); return m_data.as_float  = rhs; }
    inline double& operator=(const double& rhs) { massert(m_type == L"double", "Writting incompatible value to MetaData."); return m_data.as_double = rhs; }

    inline Vec2<uint8>& operator=(const Vec2<uint8>& rhs) { massert(m_type == L"v2uint8", "Writting incompatible value to MetaData."); return m_data.as_v2uint8 = rhs; }
    inline Vec2<float>& operator=(const Vec2<float>& rhs) { massert(m_type == L"v2float", "Writting incompatible value to MetaData."); return m_data.as_v2float = rhs; }

    inline const bool&  as_bool()  const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_bool; }

    inline const int8&  as_int8()  const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int8; }
    inline const int16& as_int16() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int16; }
    inline const int32& as_int32() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int32; }
    inline const int64& as_int64() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int64; }

    inline const uint8&  as_uint8()  const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint8; }
    inline const uint16& as_uint16() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint16; }
    inline const uint32& as_uint32() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint32; }
    inline const uint64& as_uint64() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint64; }

    inline const float&  as_float()  const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_float; }
    inline const double& as_double() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_double; }

    inline const Vec2<uint8>& as_v2uint8() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_v2uint8; }
    inline const Vec2<float>& as_v2float() const { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_v2float; }

    inline bool&  as_bool() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_bool; }

    inline int8&  as_int8()  { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int8; }
    inline int16& as_int16() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int16; }
    inline int32& as_int32() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int32; }
    inline int64& as_int64() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_int64; }

    inline uint8&  as_uint8()  { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint8; }
    inline uint16& as_uint16() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint16; }
    inline uint32& as_uint32() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint32; }
    inline uint64& as_uint64() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_uint64; }

    inline float&  as_float()  { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_float; }
    inline double& as_double() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_double; }

    inline Vec2<uint8>& as_v2uint8() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_v2uint8; }
    inline Vec2<float>& as_v2float() { massert(!m_type.empty(), "Accessing uninitialized MetaData."); return m_data.as_v2float; }

    inline void init_bool(bool value) { m_type = L"bool";  m_data.as_bool = value; }

    inline void init_int8 (int8  value) { m_type = L"int8";  m_data.as_int8  = value; }
    inline void init_int16(int16 value) { m_type = L"int16"; m_data.as_int16 = value; }
    inline void init_int32(int32 value) { m_type = L"int32"; m_data.as_int32 = value; }
    inline void init_int64(int64 value) { m_type = L"int64"; m_data.as_int64 = value; }

    inline void init_uint8 (uint8 value)  { m_type = L"uint8";  m_data.as_uint8  = value; }
    inline void init_uint16(uint16 value) { m_type = L"uint16"; m_data.as_uint16 = value; }
    inline void init_uint32(uint32 value) { m_type = L"uint32"; m_data.as_uint32 = value; }
    inline void init_uint64(uint64 value) { m_type = L"uint64"; m_data.as_uint64 = value; }

    inline void init_float (float value)  { m_type = L"float"; m_data.as_float = value; }
    inline void init_double(double value) { m_type = L"double"; m_data.as_double = value; }

    inline void init_v2uint8(const Vec2<uint8>& value) { m_type = L"v2uint8"; m_data.as_v2uint8 = value; }
    inline void init_v2float(const Vec2<float>& value) { m_type = L"v2float"; m_data.as_v2float = value; }

private:

    std::wstring m_type;
    MetaData_t m_data;
};

