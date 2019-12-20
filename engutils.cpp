#include "engutils.hpp"

namespace eng {

int ustrlen(const u8 *str)
{
    int i = 0;

    while (str[i] != 0)
    {
        ++i;
    }

    return i;
}

void strToUStr(
    const char *str,
    u8 *dest,
    const  u8 *subs,
    char del
) {
    int subIndex = 0;
    int i = 0;
    char c = 0;

    if (subs == nullptr)
    {
        while ((c = str[i]) != 0)
        {
            dest[i] = (u8)c;
            i++;
        }
    }
    else
    {
        while ((c = str[i]) != 0)
        {
            if (c == del)
            {
                dest[i] = subs[subIndex++];
            }
            else
            {
                dest[i] = (u8)c;
            }

            i++;
        }
    }

    dest[i] = 0;
}

void strToUStrLen(
    const char *str,
    u8 *dest,
    u32 len,
    const u8 *subs,
    char del
) {
    int subIndex = 0;

    if (subs == nullptr)
    {
        for (int i = 0; i < len; i++)
        {
            dest[i] = (u8)(str[i]);
        }
    }
    else
    {
        for (int i = 0; i < len; i++)
        {
            char c = str[i];

            if (c == del)
            {
                dest[i] = subs[subIndex++];
            }
            else
            {
                dest[i] = (u8)c;
            }
        }
    }
}

bool strcpymx(char *dest, const char *src, int mx)
{
    bool clipped = false;

    int i;
    for (i = 0; i < mx; i++)
    {
        char c = src[i];

        if (c == 0)
        {
            clipped = true;
            break;
        }

        dest[i] = c;
    }

    dest[i] = 0;
    return clipped;
}

bool ustrcpymx(u8 *dest, const u8 *src, int mx)
{
    bool clipped = false;

    int i;
    for (i = 0; i < mx; i++)
    {
        u8 c = src[i];

        if (c == 0)
        {
            clipped = true;
            break;
        }

        dest[i] = c;

    }

    dest[i] = 0;
    return clipped;
}

}
