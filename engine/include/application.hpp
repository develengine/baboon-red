#pragma once

#include <SDL2/SDL.h>
#include <functional>

namespace Application {

extern SDL_Window *window;
extern SDL_GLContext context;
extern bool running;

void init();
void setKeyCallback(std::function<void(SDL_Event&, bool)> f);
void setMouseMotionCallback(std::function<void(SDL_Event&)> f);
void pollEvents();
void close();

}
