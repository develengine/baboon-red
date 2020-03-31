#pragma once

#include "eng.h"

#include <iostream>
#include <cstring>


namespace eng {

int ustrlen(const u8 *str);

void strToUStr(
    const char *str,
    u8 *dest,
    const  u8 *subs = nullptr,
    char del = '%'
);

void strToUStrLen(
    const char *str,
    u8 *dest,
    u32 len,
    const u8 *subs = nullptr,
    char del = '%'
);

bool strcpymx(char *dest, const char *src, int mx);
bool ustrcpymx(u8 *dest, const u8 *src, int mx);

template<typename T, u32 N>
class WrapStack
{
public:

    int dataPointer = 0;
    int pushCounter = 0;
    T data[N];

    inline int size() const
    {
        return pushCounter < N ? pushCounter : N;
    }

    inline int position(int index) const
    {
        return (N + ((dataPointer - index) % N)) % N;
    }

    void push(const T &e)
    {
        dataPointer = (dataPointer + 1) % N;
        data[dataPointer] = e;
        ++pushCounter;
    }

    T& get()
    {
        dataPointer = (dataPointer + 1) % N;
        ++pushCounter;
        return data[dataPointer];
    }

    void clear()
    {
        dataPointer = 0;
        pushCounter = 0;
    }

    inline T& operator[] (int index)
    {
        return data[position(index)];
    }

    inline const T& operator[] (int index) const
    {
        return data[position(index)];
    }
};

template<typename T, u32 N>
class WrapQueue
{
public:

    int dataPointer = 0;
    int pushCounter = 0;
    T data[N];

    inline int size() const
    {
        return pushCounter < N ? pushCounter : N;
    }

    inline int position(int index) const
    {
        return (dataPointer + index) % N;
    }

    void push(const T &e)
    {
        dataPointer = pushCounter < N ? dataPointer : (dataPointer + 1) % N;
        data[pushCounter++ % N] = e;
    }

    T& get()
    {
        dataPointer = pushCounter < N ? dataPointer : (dataPointer + 1) % N;
        return data[pushCounter++ % N];
    }

    void clear()
    {
        dataPointer = 0;
        pushCounter = 0;
    }

    inline T& operator[] (int index)
    {
        return data[position(index)];
    }

    inline const T& operator[] (int index) const
    {
        return data[position(index)];
    }
};

inline void swapBytes(u8 *b)
{
    u8 buffer[] = { b[3], b[2], b[1], b[0] };
    memcpy(b, buffer, 4);
}

}
