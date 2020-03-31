#include "input.hpp"

#include "textedit.hpp"
#include "console.hpp"
#include "application.hpp"
#include "application.hpp"

namespace Input {

const u32 ScanCodes[]
{
    SDL_SCANCODE_W,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_SPACE,
    SDL_SCANCODE_LSHIFT,
    SDL_SCANCODE_UP,
    SDL_SCANCODE_DOWN,
    SDL_SCANCODE_LEFT,
    SDL_SCANCODE_RIGHT,
    SDL_SCANCODE_K,
    SDL_SCANCODE_J,
    SDL_SCANCODE_H,
    SDL_SCANCODE_L,
    SDL_SCANCODE_LSHIFT
};

bool keyStates[KEY_COUNT];

bool cursorEnabled = false;

u32 windowType = 0;
#ifdef _WIN32
constexpr u32 FULLSCREEN = SDL_WINDOW_FULLSCREEN;
#else
constexpr u32 FULLSCREEN = SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

void keyCallback(SDL_Event &event, bool down)
{
    SDL_Scancode scancode = event.key.keysym.scancode;

    if (!Console::active)
    {
        for (int i = 0; i < KEY_COUNT; i++)
        {
            if (scancode == ScanCodes[i])
            {
                keyStates[i] = down;
                break;
            }
        }
    }

    if (!down) return;

    switch (scancode)
    {
        // For debug mostly
        case SDL_SCANCODE_F9:

            Console::write("Debug line!");

            break;

        case SDL_SCANCODE_F11:

            if (windowType == 0)
            {
                windowType = FULLSCREEN;
            }
            else
            {
                windowType = 0;
            }

            if (SDL_SetWindowFullscreen(Application::window, windowType) != 0)
            {
                std::cerr << "Failed to change window! Error: " << SDL_GetError() << '\n';
            }

            break;

        case SDL_SCANCODE_LALT:

            SDL_ShowCursor(cursorEnabled ? SDL_FALSE : SDL_TRUE);
            SDL_SetRelativeMouseMode(cursorEnabled ? SDL_TRUE : SDL_FALSE);
            cursorEnabled = !cursorEnabled;

            break;

        case SDL_SCANCODE_ESCAPE:

            Application::running = false;

            break;

        case SDL_SCANCODE_RALT:

            Console::active = !Console::active;

            if (Console::active)
            {
                TextEdit::setTarget(
                    Console::lineInProgress,
                    CONSOLE_INPUT_LINE_SIZE,
                    &Console::inputCallback
                );
                Console::inputActive = true;
            }
            else
            {
                TextEdit::terminate();
            }

            break;

        case SDL_SCANCODE_RETURN:

            TextEdit::enter();

            break;

        case SDL_SCANCODE_BACKSPACE:

            TextEdit::deleteLeft();

            break;

        case SDL_SCANCODE_DELETE:

            TextEdit::deleteRight();

            break;

        case SDL_SCANCODE_LEFT:

            TextEdit::moveLeft();

            break;

        case SDL_SCANCODE_RIGHT:

            TextEdit::moveRight();

            break;

        case SDL_SCANCODE_HOME:

            TextEdit::moveStart();

            break;

        case SDL_SCANCODE_END:

            TextEdit::moveEnd();

            break;

        default : return;
    }
}

}
