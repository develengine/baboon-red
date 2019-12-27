#pragma once

#include "eng.h"

#include "math.hpp"
#include "textedit.hpp"

namespace Console {

extern const int LINE_LENGTH;
extern const int LINE_COUNT;
extern const int VISIBLE_LINE_COUNT;
extern const int DEVICE_MEMORY_OFFSET;
// extern const int INPUT_LINE_SIZE;
#define CONSOLE_INPUT_LINE_SIZE 128
extern const int MAX_ARG_COUNT;

extern u8 lineInProgress[CONSOLE_INPUT_LINE_SIZE];
extern bool inputActive;

extern bool active;

extern eng::Vec2f consolePosition;

extern int lineOffset;

void write(const u8 *text);
void write(const char *text);
void inputCallback(TextEdit::ReturnState s);
void render();

}
