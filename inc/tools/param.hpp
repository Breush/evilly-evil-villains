#pragma once

// Getter
#define PARAMG(type, name, getter)                      \
    private:                                            \
        type name;                                      \
    public:                                             \
        type  getter() const noexcept { return name; }  \
        type& getter()       noexcept { return name; }

// Getter and setter
#define PARAMGS(type, name, getter, setter)                 \
    private:                                                \
        type name;                                          \
    public:                                                 \
        type  getter() const noexcept { return name; }      \
        type& getter()       noexcept { return name; }      \
        void setter(type value) noexcept { name = value; }

// Getter, setter and update call
#define PARAMGSU(type, name, getter, setter, update)            \
    private:                                                    \
        type name;                                              \
    public:                                                     \
        type  getter() const noexcept { return name; }          \
        type& getter()       noexcept { return name; }          \
        void setter(type value) { name = value; update(); }
