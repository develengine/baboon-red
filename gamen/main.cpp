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
#include "input.hpp"

#include "gamenoglcore.hpp"

float lines[]
{
     0.0f, 0.0f, 1.0f, 1.0f,
    -1.0f, 0.5f, 0.0f, 0.5f
};

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

float boxColliderLines[]
{
    0.0f, 0.0f,  1.0f, 0.0f,
    1.0f, 0.0f,  1.0f, 1.0f,
    1.0f, 1.0f,  0.0f, 1.0f,
    0.0f, 1.0f,  0.0f, 0.0f
};

inline float collisionLineOnLine(
    float oax, float oay, float obx, float oby,
    float cax, float cay, float cbx, float cby,
    float vxn, float vyn, float d
) {
    // oax and oay are 0
    float obxTranslated = obx - oax;
    float obyTranslated = oby - oay;
    float caxTranslated = cax - oax;
    float cayTranslated = cay - oay;
    float cbxTranslated = cbx - oax;
    float cbyTranslated = cby - oay;

    // oax and oay still 0
    float obxRotated = obxTranslated * vxn + obyTranslated * vyn;
    float obyRotated = obyTranslated * vxn - obxTranslated * vyn;
    float caxRotated = caxTranslated * vxn + cayTranslated * vyn;
    float cayRotated = cayTranslated * vxn - caxTranslated * vyn;
    float cbxRotated = cbxTranslated * vxn + cbyTranslated * vyn;
    float cbyRotated = cbyTranslated * vxn - cbxTranslated * vyn;

    #define CLOL_RETURN    \
        if (d1 < d2)       \
        {                  \
            if (d < d1)    \
            {              \
                return d;  \
            }              \
            else           \
            {              \
                return d1; \
            }              \
        }                  \
        else               \
        {                  \
            if (d < d2)    \
            {              \
                return d;  \
            }              \
            else           \
            {              \
                return d2; \
            }              \
        }

    if (obyRotated > 0)
    {
        if (cayRotated > obyRotated)
        {
            if (cbyRotated > obyRotated)
            {
                return d;
            }
            else if (cbyRotated > 0)
            {
                // ob, cb
                float d1 = caxRotated + ((cbxRotated - caxRotated) / (cayRotated - cbyRotated)) * (cayRotated - obyRotated) - obxRotated;
                float d2 = cbxRotated - (obxRotated / obyRotated) * cbyRotated;

                CLOL_RETURN
            }
            else
            {
                // oa, ob
                float ratio = (cbxRotated - caxRotated) / (cayRotated - cbyRotated);
                float d1 = caxRotated + (caxRotated - obxRotated) * ratio - obxRotated;
                float d2 = caxRotated + cay * ratio;

                CLOL_RETURN
            }
        }
        else if (cayRotated > 0)
        {
            if (cbyRotated > cayRotated)
            {
                return d;
            }
            else if (cbyRotated > 0)
            {
                // ca, cb
                float ratio = obxRotated / obyRotated;
                float d1 = caxRotated - cayRotated * ratio;
                float d2 = cbxRotated - cbyRotated * ratio;

                CLOL_RETURN
            }
            else
            {
                // oa, ca
                float d1 = caxRotated - (obxRotated / obyRotated) * cayRotated;
                float d2 = caxRotated + ((cbxRotated - caxRotated) / (cayRotated - cbyRotated)) * cayRotated;

                CLOL_RETURN
            }
        }
    }
//     else
//     {
//         if (cayRotated < obyRotated)
//         {
//             if (cbyRotated < obyRotated)
//             {
//                 return d;
//             }
//             else if (cbyRotated < 0)
//             {
//                 // ob, cb
//             }
//             else
//             {
//                 // oa, ob
//             }
//         }
//         else if (cayRotated < 0)
//         {
//             if (cbyRotated < cayRotated)
//             {
//                 return d;
//             }
//             else if (cbyRotated < 0)
//             {
//                 // ca, cb
//             }
//             else
//             {
//                 // oa, ca
//             }
//         }
//     }

    return d;
}


float clipMovement(
    float vx, float vy,
    float ox, float oy,
    float sx, float sy
) {
    return 0.0f;
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
    Application::setKeyCallback(&Input::keyCallback);

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


    glGenVertexArrays(1, &VAO(LINE));
    glBindVertexArray(VAO(LINE));

    glGenBuffers(1, &BUF(LINE_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(LINE_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(lines), lines, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
    glLineWidth(4.0f);

    while (Application::running)
    {
        Application::pollEvents();

        float speed = 5.0f * deltaTime;

        if (KEY(LEFT))
        {
            rPos[0] -= speed * 0.5;
        }

        if (KEY(RIGHT))
        {
            rPos[0] += speed * 0.5;
        }

        if (KEY(UP))
        {
            rPos[1] += speed * 0.5;
        }

        if (KEY(DOWN))
        {
            rPos[1] -= speed * 0.5;
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


        eng::Vec2f newRectPos = (rPos - cameraPosition) * baseScale;
        eng::Vec2f newRectScale = rScl * baseScale;
        eng::Vec2f torsoPos = newRectPos + eng::Vec2f(0.0f, baseScale[1] * 0.5f);

        glBindVertexArray(VAO(CLASSIC));
        glUseProgram(PRG(GAMEN));

        glUniform2fv(UNI(GAMEN_SCALE), 1, newRectScale.data);

        glUniform2fv(UNI(GAMEN_POSITION), 1, torsoPos.data);
        glUniform2f(UNI(GAMEN_TEX_OFFSET), singleTileSpan * 3, singleTileSpan * 6);
        glDrawElements(GL_TRIANGLES, sizeof(rectangleIndices) / sizeof(u32), GL_UNSIGNED_INT, (void*)(0));

        glUniform2fv(UNI(GAMEN_POSITION), 1, newRectPos.data);
        glUniform2f(UNI(GAMEN_TEX_OFFSET), singleTileSpan * 3, singleTileSpan * 7);
        glDrawElements(GL_TRIANGLES, sizeof(rectangleIndices) / sizeof(u32), GL_UNSIGNED_INT, (void*)(0));


        eng::Vec2f linesPosition(0.0f, 0.0f);
        eng::Vec2f linesRelativePosition = (linesPosition - cameraPosition) * baseScale;
        eng::Vec2f linesScale(1.0f, 1.0f);
        eng::Vec2f linesRelativeScale = linesScale * baseScale;

        glBindVertexArray(VAO(LINE));
        glUseProgram(ENG_PRG(RECT_COL));

        glUniform2fv(ENG_UNI(RECT_COL_POSITION), 1, linesRelativePosition.data);
        glUniform2fv(ENG_UNI(RECT_COL_SCALE), 1, linesRelativeScale.data);
        glUniform4f(ENG_UNI(RECT_COL_COLOR), 9.0f, 0.1f, 0.2f, 1.0f);

        glDrawArrays(GL_LINES, 0, sizeof(lines) / sizeof(float));


        eng::Vec2f pointB(cos(timePassed), sin(timePassed));

        glBindVertexArray(ENG_VAO(DUMMY));
        glUseProgram(ENG_PRG(LINE));

        glUniform2f(ENG_UNI(LINE_VERT0), 0.0f, 0.0f);
        glUniform2fv(ENG_UNI(LINE_VERT1), 1, pointB.data);
        glUniform4f(ENG_UNI(LINE_COLOR), 0.1f, 0.9f, 0.2f, 1.0f);

        glDrawArrays(GL_LINES, 0, 2);


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

