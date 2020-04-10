#include "application.hpp"

#include <glad/glad.h>

#include <iostream>

#include "textedit.hpp"

namespace Application {

u32 initFlags = SDL_INIT_VIDEO;
SDL_Window *window = nullptr;
SDL_GLContext context;
bool running = true;

std::string windowName("EngineTech");
int windowWidth = 1080;
int windowHeight = 720;

std::function<void(SDL_Event&, bool)> keyCallback = nullptr;
std::function<void(SDL_Event&)> mouseMotionCallback = nullptr;
std::function<void(SDL_Event&, bool)> mouseButtonCallback = nullptr;
std::function<void(SDL_Event&)> mouseWheelCallback = nullptr;

void setInitFlag(u32 flag)
{
    initFlags |= flag;
}

void setKeyCallback(std::function<void(SDL_Event&, bool)> f)
{
    keyCallback = f;
}

void setMouseMotionCallback(std::function<void(SDL_Event&)> f)
{
    mouseMotionCallback = f;
}

void setMouseButtonCallback(std::function<void(SDL_Event&, bool)> f)
{
    mouseButtonCallback = f;
}

void setMouseWheelCallback(std::function<void(SDL_Event&)> f)
{
    mouseWheelCallback = f;
}

void setWindowName(const char *name)
{
    windowName = name;

    if (window != nullptr)
    {
        SDL_SetWindowTitle(window, windowName.c_str());
    }
}

void setWindowSize(int width, int height)
{
    windowWidth = width;
    windowHeight = height;

    if (window != nullptr)
    {
        SDL_SetWindowSize(window, width, height);
    }
}

void GLAPIENTRY openglCallback( GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam
) {
    bool error = type == GL_DEBUG_TYPE_ERROR;
    std::cerr << "GL: error: " << error << " type: " << type << " severity: " << severity
              << ".\nmessage: " << message << '\n';
}

SDL_Event event;
void pollEvents()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:

                running = false;

                break;

            case SDL_WINDOWEVENT:

                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:

                        glViewport(0, 0, event.window.data1, event.window.data2);

                        break;
                }

                break;

            case SDL_KEYDOWN:

                if (keyCallback != nullptr)
                {
                    keyCallback(event, true);
                }

                break;

            case SDL_KEYUP:

                if (keyCallback != nullptr)
                {
                    keyCallback(event, false);
                }

                break;

            case SDL_MOUSEMOTION:

                if (mouseMotionCallback != nullptr)
                {
                    mouseMotionCallback(event);
                }

                break;

            case SDL_MOUSEWHEEL:

                if (mouseWheelCallback != nullptr)
                {
                    mouseWheelCallback(event);
                }

                break;

            case SDL_TEXTINPUT:

//                 std::cout << event.text.text << '\n';
                TextEdit::write(event.text.text);

                break;

            case SDL_TEXTEDITING:

                std::cout << "Edit" << '\n';

                break;

            case SDL_MOUSEBUTTONDOWN:

                if (mouseButtonCallback != nullptr)
                {
                    mouseButtonCallback(event, true);
                }

                break;

            case SDL_MOUSEBUTTONUP:

                if (mouseButtonCallback != nullptr)
                {
                    mouseButtonCallback(event, false);
                }

                break;
        }
    }
}

void init()
{
    if (SDL_Init(initFlags) < 0)
    {
        std::cerr << "Failed to initialize SDL2\n";
        exit(-1);
    }

    SDL_GL_LoadLibrary(NULL);

    window = SDL_CreateWindow(
        windowName.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        windowWidth, windowHeight,
        SDL_WINDOW_OPENGL
    );

    SDL_SetWindowResizable(window, SDL_TRUE);

    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    context = SDL_GL_CreateContext(window);

    if (context == NULL)
    {
        std::cerr << "Failed to initialize context\n";
        exit(-1);
    }

    if (!gladLoadGLLoader(SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to load GLAD\n";
        exit(-1);
    }

    SDL_GL_SetSwapInterval(1);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(openglCallback, 0); 
}

void close()
{
    SDL_DestroyWindow(window);
    SDL_Quit();
}

}
