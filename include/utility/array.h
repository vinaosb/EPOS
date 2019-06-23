// EPOS Array Utility Declarations

#ifndef __array_h
#define __array_h

#include <utility/ostream.h>
#include <utility/string.h>

__BEGIN_UTIL

template<typename T, unsigned int N_ELEMENTS>
class Array
{
private:
    static const unsigned int SIZE = N_ELEMENTS * sizeof(T);

public:
    typedef T Object_Type;

public:
    Array() {}
    Array(const void * data, unsigned int size) { copy_and_pad(data, size); }
    template <typename U>
    Array(const U & o) { copy_and_pad(&o, sizeof(U)); }

    Object_Type & operator[](const size_t i) { return _data[i]; }
    const Object_Type & operator[](const size_t i) const { return _data[i]; }
    operator const Object_Type *() const { return _data; }
    operator Object_Type *() { return _data; }

    size_t search(const Object_Type & obj) {
        size_t i = 0;
        for(; i < N_ELEMENTS; i++)
            if(_data[i] == obj)
                break;
        return i;
    }

    template <typename U>
    bool operator==(const U & o) const { return (SIZE == sizeof(U)) && (!memcmp(&o, _data, SIZE)); }
    template <typename U>
    bool operator!=(const U & o) const { return !(*this == o); }

    friend OStream & operator<<(OStream & out, const Array & array) {
        out << "[";
        for(unsigned int i = 0; i < N_ELEMENTS; i++) {
            out << array[i];
            if(i < N_ELEMENTS - 1)
                out << ",";
        }
        out << "]";
        return out;
    }

    friend Debug & operator<<(Debug & out, const Array & array) {
        out << "[";
        for(unsigned int i = 0; i < N_ELEMENTS; i++) {
            out << array[i];
            if(i < N_ELEMENTS - 1)
                out << ",";
        }
        out << "]";
        return out;
    }

private:
    void copy_and_pad(const void * data, unsigned int size) {
        if(SIZE <= size)
            memcpy(_data, data, SIZE);
        else {
            memset(_data, 0, SIZE);
            memcpy(_data, data, size);
        }
    }

private:
    T _data[N_ELEMENTS];
};

__END_UTIL

#endif
