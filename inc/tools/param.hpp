#pragma once

// Getter
#define PARAMG(type, name, getter)              \
    private:                                    \
        type name;                              \
    public:                                     \
        type  getter() const { return name; }   \
        type& getter() { return name; }

#define PARAMGNR(type, name, getter)              \
    private:                                    \
        type name;                              \
    public:                                     \
        type  getter() const { return name; }

// Getter and setter
#define PARAMGS(type, name, getter, setter)         \
    private:                                        \
        type name;                                  \
    public:                                         \
        type  getter() const { return name; }       \
        type& getter() { return name; }             \
        void setter(type value) { name = value; }

// Getter, setter and update call
#define PARAMGSU(type, name, getter, setter, update)            \
    private:                                                    \
        type name;                                              \
    public:                                                     \
        type  getter() const { return name; }                   \
        type& getter() { return name; }                         \
        void setter(type value) { name = value; update(); }
