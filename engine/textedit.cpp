#include "textedit.hpp"

#include "engutils.hpp"

#include <algorithm>
#include <cstring>

namespace TextEdit {

int bufferSize = 0;
int position = 0;
u8 *target = nullptr;
std::function<void(ReturnState)> callback = nullptr;

void setTarget(u8 *t, int len, std::function<void(ReturnState)> cb)
{

    if (target != nullptr && callback != nullptr)
    {
        callback(TERMINATION);
    }

    target = t;
    bufferSize = len;
    position = eng::ustrlen(target);
    callback = cb;
}

void write(const char *text)
{
    if (target == nullptr) return;

    int targetLen = eng::ustrlen(target);
    int moveDistance = std::min(
        (int)strlen(text),
        bufferSize - 1 - targetLen
    );

    for (int i = targetLen; i >= position; i--)
    {
        target[i + moveDistance] = target[i];
    }

    for (int i = 0; i < moveDistance; i++)
    {
        target[position + i] = (u8)(text[i]);
    }

    position += moveDistance;
}

void moveLeft()
{
    if (target == nullptr) return;

    position = std::max(position - 1, 0);
}

void moveRight()
{
    if (target == nullptr) return;

    position = std::min(position + 1, eng::ustrlen(target));
}

void moveStart()
{
    if (target == nullptr) return;

    position = 0;
}

void moveEnd()
{
    if (target == nullptr) return;

    position = eng::ustrlen(target);
}

void deleteLeft()
{
    if (target == nullptr) return;

    if (position > 0)
    {
        int p = position;
        u8  c;

        do {
            c = target[p];
            target[p - 1] = c;
            ++p;
        }
        while (c != 0);

        --position;
    }
}

void deleteRight()
{
    if (target == nullptr) return;

    if (target[position] != 0)
    {
        int p = position;
        u8  c;

        do {
            c = target[p + 1];
            target[p] = c;
            ++p;
        }
        while (c != 0);
    }
}

void enter()
{
    if (target == nullptr) return;

    target = nullptr;

    if (callback != nullptr)
    {
        callback(ENTRY);
    }
}

void terminate()
{
    if (target == nullptr) return;

    target = nullptr;

    if (callback != nullptr)
    {
        callback(TERMINATION);
    }
}

}

