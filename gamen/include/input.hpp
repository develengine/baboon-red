#pragma once

#include "eng.h"

#include <SDL2/SDL.h>

namespace Input {

enum KeySigns
{
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_SPACE,
    KEY_SHIFT,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_K,
    KEY_J,
    KEY_H,
    KEY_L,

    KEY_COUNT
};

void keyCallback(SDL_Event &event, bool down);

extern const u32 ScanCodes[];

extern bool keyStates[KEY_COUNT];

extern bool cursorEnabled;
extern u32 windowType;

}

#define KEY(x) ( Input::keyStates[ Input::KEY_ ## x ] )

