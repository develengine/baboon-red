#pragma once

#include "eng.h"
#include <SDL2/SDL.h>
#include <functional>

namespace Application {

extern SDL_Window *window;
extern SDL_GLContext context;
extern bool running;

void setInitFlag(u32 flag);
void init();
void setKeyCallback(std::function<void(SDL_Event&, bool)> f);
void setMouseMotionCallback(std::function<void(SDL_Event&)> f);
void setMouseButtonCallback(std::function<void(SDL_Event&, bool)> f);
void setMouseWheelCallback(std::function<void(SDL_Event&)> f);
void setWindowName(const char *name);
void setWindowSize(int width, int height);
void pollEvents();
void close();

}
