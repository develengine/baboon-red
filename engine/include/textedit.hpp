#pragma once

#include "eng.h"

#include <functional>

namespace TextEdit {

enum ReturnState
{
    ENTRY,
    TERMINATION
};

extern int position;

void setTarget(u8 *t, int len, std::function<void(ReturnState)> cb);
void write(const char *text);
void moveLeft();
void moveRight();
void moveStart();
void moveEnd();
void deleteLeft();
void deleteRight();
void enter();
void terminate();

}
