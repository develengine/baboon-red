/* Other TODO:
 - Directional shadows
 - Point lights
 - Point shadows
 - Animation
 - Sound
 - Collisions
 - Sky boxes
*/

#include "eng.h"

#include "application.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stb_image.h>

#include <iostream>
#include <string>
#include <chrono>

#include <cmath>
#include <climits>

#include "math.hpp"
#include "textures.hpp"
#include "shaders.hpp"
#include "engutils.hpp"
#include "textedit.hpp"
#include "oglcore.hpp"
#include "commander.hpp"
#include "text.hpp"
#include "ez2d.hpp"
#include "console.hpp"

#include "gamenoglcore.hpp"

float rectangleVertices[]
{
     0.0f, 0.0f, 0.001f, 0.001f,
     0.0f,-1.0f, 0.001f, 0.999f,
     1.0f, 0.0f, 0.999f, 0.001f,
     1.0f,-1.0f, 0.999f, 0.999f
};

u32 rectangleIndices[]
{
    0, 1, 2,
    2, 1, 3
};

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

#define KEY(x) ( keyStates[ KEY_ ## x ] )

u32 windowType = 0;
#ifdef _WIN32
const u32 FULLSCREEN = SDL_WINDOW_FULLSCREEN;
#else
const u32 FULLSCREEN = SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

bool cursorEnabled = false;

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

int main(int argc, char *argv[])
{
    bool chunkEdited = false;
    u32 testChunk[32 * 32] = { 0 };

    for (int i = 0; i < 32 * 32; i++)
    {
        testChunk[i] = 7 * 32 + 1;
    }

    Commander::addCommand("kek", [] (int c, char *v[])
    {
        for (int i = 0; i < c; i++)
        {
            std::cout << v[i] << '\n';
            Console::write(v[i]);
        }
    });

    Application::init();
    Application::setKeyCallback(&keyCallback);

    SDL_StartTextInput();
//     SDL_StopTextInput();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Vao stuff

    glGenVertexArrays(1, &VAO(CLASSIC));
    glBindVertexArray(VAO(CLASSIC));

    glGenBuffers(1, &BUF(CLASSIC_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(CLASSIC_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &BUF(CLASSIC_IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUF(CLASSIC_IBO));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    glGenVertexArrays(1, &VAO(CHUNKY));
    glBindVertexArray(VAO(CHUNKY));

    glGenBuffers(1, &BUF(CHUNKY_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(CHUNKY_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &BUF(CHUNKY_DATA));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(CHUNKY_DATA));
    glBufferData(GL_ARRAY_BUFFER, sizeof(testChunk), testChunk, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(u32), (void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(2, 1);

    glGenBuffers(1, &BUF(CHUNKY_IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUF(CHUNKY_IBO));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(rectangleIndices), rectangleIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);


// Texture stuff

    int width, height, channelCount;
    stbi_set_flip_vertically_on_load(false);
    u8 *data = stbi_load("../cringe/tileset.png", &width, &height, &channelCount, STBI_rgb_alpha);

    if (!data)
    {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    Texture::Parameters parameters
    {
        data,
        width, height,
        GL_REPEAT, GL_REPEAT,
        GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,
        GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
        true
    };

    Texture::generate(TEX(CLASSIC), parameters);

    free(data);


// Shader stuff

    Shader::generate(
        PRG(GAMEN),
        SHD(GAMEN_VERTEX), SHD(GAMEN_FRAGMENT),
        Shader::load("shaders/gamenshader.vert"),
        Shader::load("shaders/gamenshader.frag")
    );

    UNI(GAMEN_POSITION) = glGetUniformLocation(PRG(GAMEN), "u_position");
    UNI(GAMEN_SCALE) = glGetUniformLocation(PRG(GAMEN), "u_scale");
    UNI(GAMEN_TEX_OFFSET) = glGetUniformLocation(PRG(GAMEN), "u_texOffset");
    UNI(GAMEN_TEX_SPAN) = glGetUniformLocation(PRG(GAMEN), "u_texSpan");

    glUseProgram(PRG(GAMEN));
    glUniform1i(glGetUniformLocation(PRG(GAMEN), "u_sampler"), 0);

    Shader::generate(
        PRG(CHUNKY),
        SHD(CHUNKY_VERTEX), SHD(CHUNKY_FRAGMENT),
        Shader::load("shaders/chunky.vert"),
        Shader::load("shaders/chunky.frag")
    );

    UNI(CHUNKY_POSITION) = glGetUniformLocation(PRG(CHUNKY), "u_position");
    UNI(CHUNKY_SCALE) = glGetUniformLocation(PRG(CHUNKY), "u_scale");

    glUseProgram(PRG(CHUNKY));
    glUniform1i(glGetUniformLocation(PRG(CHUNKY), "u_sampler"), 0);

// Init other stuff

    Text::init();
    EZ2D::init();

// Doo doo

    eng::Vec2f rPos(-0.5f, 0.5f);
    eng::Vec2f rScl(0.5f, 0.5f);

    eng::Vec2f cameraPosition(0.0f, 0.0f);
    float cameraZoom = 1.0f;
    auto tp1 = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;
    double timePassed = 0.0;

    int wWidth, wHeight;
    SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

    Application::setMouseButtonCallback( [&] (SDL_Event &event, bool down)
    {
        SDL_MouseButtonEvent &be = event.button;

        float ratio   = (float)wWidth / (float)wHeight;
        float screenX = (((float)(be.x) / (float)wWidth) - 0.5f) * 2 * ratio;
        float screenY = -(((float)(be.y) / (float)wHeight) - 0.5f) * 2;

        eng::Vec2f worldPosition(screenX / cameraZoom + cameraPosition[0], screenY / cameraZoom + cameraPosition[1]);

        std::cout << "Position: " << worldPosition[0] << ", "  << worldPosition[1] << '\n'
                  << "Button: "   << (u32)(be.button) << '\n'
                  << "Clicks: "   << (u32)(be.clicks) << '\n'
                  << '\n';

        if (worldPosition[0] > 0.0f && worldPosition[0] < 16.0f
        &&  worldPosition[1] < 0.0f && worldPosition[1] >-16.0f)
        {
            int x = floor(worldPosition[0] * 2.0f);
            int y = abs(floor(worldPosition[1] * 2.0f)) - 1;

            std::cout << x << ", " << y << '\n' << '\n';

            testChunk[y * 32 + x] = y * 32 + x;
            chunkEdited = true;
        }
    });


    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TEX(CLASSIC));

    float singleTileSpan = 8.0f / 256.0f;

    glUseProgram(PRG(GAMEN));
    glUniform2f(UNI(GAMEN_TEX_SPAN), singleTileSpan, singleTileSpan);

    glDisable(GL_DEPTH_TEST);

    while (Application::running)
    {
        Application::pollEvents();

        float speed = 5.0f * deltaTime;

        if (KEY(LEFT))
        {
            rPos[0] -= speed;
        }

        if (KEY(RIGHT))
        {
            rPos[0] += speed;
        }

        if (KEY(UP))
        {
            rPos[1] += speed;
        }

        if (KEY(DOWN))
        {
            rPos[1] -= speed;
        }

        if (KEY(A))
        {
            cameraPosition[0] -= speed;
        }

        if (KEY(D))
        {
            cameraPosition[0] += speed;
        }

        if (KEY(W))
        {
            cameraPosition[1] += speed;
        }

        if (KEY(S))
        {
            cameraPosition[1] -= speed;
        }

        if (KEY(SPACE))
        {
            cameraZoom -= speed * 0.1;
        }

        if (KEY(SHIFT))
        {
            cameraZoom += speed * 0.1;
        }

        SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

        float widthRatio = (float)wHeight / (float)wWidth;
        eng::Vec2f baseScale(widthRatio * cameraZoom, cameraZoom);

        eng::Vec2f newRectPos = (rPos - cameraPosition) * baseScale;
        eng::Vec2f newRectScale = rScl * baseScale;

        glClearColor(0.35f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        eng::Vec2f chunkPosition(0.0f, 0.0f);
        eng::Vec2f chunkRelativePosition = (chunkPosition - cameraPosition) * baseScale;
        eng::Vec2f chunkScale(16.0f, 16.0f);
        eng::Vec2f chunkRelativeScale = chunkScale * baseScale;

        glBindVertexArray(VAO(CHUNKY));
        glUseProgram(PRG(CHUNKY));

        if (chunkEdited)
        {
            glBindBuffer(GL_ARRAY_BUFFER, BUF(CHUNKY_DATA));
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(testChunk), testChunk);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            chunkEdited = false;
        }

        glUniform2fv(UNI(CHUNKY_POSITION), 1, chunkRelativePosition.data);
        glUniform2fv(UNI(CHUNKY_SCALE), 1, chunkRelativeScale.data);

        glDrawElementsInstanced(GL_TRIANGLES, sizeof(rectangleIndices) / sizeof(u32), GL_UNSIGNED_INT, (void*)(0), 32 * 32);


        glBindVertexArray(VAO(CLASSIC));
        glUseProgram(PRG(GAMEN));

        glUniform2fv(UNI(GAMEN_POSITION), 1, newRectPos.data);
        glUniform2fv(UNI(GAMEN_SCALE), 1, newRectScale.data);
        glUniform2f(UNI(GAMEN_TEX_OFFSET), singleTileSpan * 0, singleTileSpan * 2);

        glDrawElements(GL_TRIANGLES, sizeof(rectangleIndices) / sizeof(u32), GL_UNSIGNED_INT, (void*)(0));


        if (Console::active)
        {
            Console::render();
        }


        SDL_GL_SwapWindow(Application::window);

        auto tp2 = std::chrono::high_resolution_clock::now();
        deltaTime = (float)((tp2 - tp1).count()) / 1000000000.f;
        timePassed += deltaTime;
        tp1 = tp2;
    }

    Shader::destroy(PRG(GAMEN), SHD(GAMEN_VERTEX), SHD(GAMEN_FRAGMENT));
    Shader::destroy(PRG(CHUNKY), SHD(CHUNKY_VERTEX), SHD(CHUNKY_FRAGMENT));

    glDeleteTextures(1, &TEX(CLASSIC));

    glDeleteBuffers(1, &BUF(CLASSIC_VBO));
    glDeleteBuffers(1, &BUF(CLASSIC_IBO));
    glDeleteVertexArrays(1, &VAO(CLASSIC));

    glDeleteBuffers(1, &BUF(CHUNKY_VBO));
    glDeleteBuffers(1, &BUF(CHUNKY_IBO));
    glDeleteBuffers(1, &BUF(CHUNKY_DATA));
    glDeleteVertexArrays(1, &VAO(CHUNKY));

    Text::close();
    EZ2D::close();

    Application::close();

    return 0;
}

