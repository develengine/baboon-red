#include "eng.h"

#include "application.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stb_image.h>

#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include <cmath>
#include <climits>

#include "math.hpp"
#include "textures.hpp"
#include "shaders.hpp"
#include "engutils.hpp"
#include "textedit.hpp"
#include "oglcore.hpp"
#include "jammerglcore.hpp"
#include "commander.hpp"
#include "text.hpp"
#include "ez2d.hpp"
#include "console.hpp"
#include "soundboy.hpp"
#include "models.hpp"

enum KeySigns
{
    KEY_W,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_SPACE,
    KEY_SHIFT,

    KEY_COUNT
};

const u32 ScanCodes[]
{
    SDL_SCANCODE_W,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_SPACE,
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

enum Instruction
{
    INS_HOP,
    INS_LEAP,
    INS_TURN_LEFT,
    INS_TURN_RIGHT,

    INS_NONE
};

void administerInput(Instruction instruction);

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

        case SDL_SCANCODE_UP:
        case SDL_SCANCODE_K:
            administerInput(INS_JUMP);
            break;

//         case SDL_SCANCODE_LEFT:
        case SDL_SCANCODE_L:
            administerInput(INS_TURN_LEFT);
            break;

//         case SDL_SCANCODE_RIGHT:
        case SDL_SCANCODE_H:
            administerInput(INS_TURN_RIGHT);
            break;

        default : return;
    }
}


struct Asset
{
    u32 vao;
    u32 vbo, ibo;
    u32 indexCount;
    u32 texture;
};

enum AssetId
{
    ASS_FROG,
    ASS_LILYPAD,
    ASS_ROCK,
    ASS_SHADOW,

    ASS_COUNT
};

Asset assets[ASS_COUNT];

void loadAssets()
{
    Models::InterleavedModelData data;
    Models::loadInterleavedModel(data, "jammer/res/models.bin");

    if (ASS_COUNT != data.modelCount)
    {
        std::cerr << "Incorect number of assets\n";
        exit(-1);
    }

    stbi_set_flip_vertically_on_load(true);

    for (int i = 0; i < data.modelCount; i++)
    {
        Asset &ass = assets[i];

        glGenVertexArrays(1, &(ass.vao));
        glBindVertexArray(ass.vao);
    
        glGenBuffers(1, &(ass.vbo));
        glBindBuffer(GL_ARRAY_BUFFER, ass.vbo);
        glBufferData(GL_ARRAY_BUFFER, data.vertexCounts[i], data.vertices[i], GL_STATIC_DRAW);
    
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    
        glGenBuffers(1, &(ass.ibo));
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ass.ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indexCounts[i], data.indices[i], GL_STATIC_DRAW);

        ass.indexCount = data.indexCounts[i];
    
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


        int width, height, channelCount;
        u8 *image = stbi_load(data.textures[i], &width, &height, &channelCount, STBI_rgb_alpha);
    
        if (!image)
        {
            std::cerr << "Failed to load image:" << data.textures[i] << '\n';
            exit(-1);
        }
    
        Texture::Parameters parameters
        {
            image,
            width, height,
            GL_REPEAT, GL_REPEAT,
            GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,
            GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
            true
        };
    
        Texture::generate(ass.texture, parameters);
    
        free(image);
    }

    data.free();
}


void freeAssets()
{
    for (Asset &ass : assets)
    {
        glDeleteTextures(1, &(ass.texture));
        glDeleteBuffers(1, &(ass.vbo));
        glDeleteBuffers(1, &(ass.ibo));
        glDeleteVertexArrays(1, &(ass.vao));
    }
}






const u32 MAP_X = 24;
const u32 MAP_Y = 12;
const u32 MAP_Z = 24;

inline int getTileIndex(int x, int y, int z)
{
    return MAP_X * MAP_Y * ((MAP_Z / 2) + z) + MAP_X * y + (MAP_X / 2) + x;
}

enum Direction
{
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST
};

eng::Vector<2, int> moveFromDirection(Direction dir)
{
    switch (dir)
    {
        case DIR_NORTH:
            return { 0,-1 };
        case DIR_EAST:
            return { 1, 0 };
        case DIR_SOUTH:
            return { 0, 1 };
        case DIR_WEST:
            return {-1, 0 };
    }
}

Direction rotate(Direction orientation, bool left, int turnCount)
{
    int temp = orientation + (left ? -1 : 1) * turnCount;

    if (temp < 0)
    {
        return (Direction)((4 - (abs(temp) % 4)) % 4);
    }

    return (Direction)(temp % 4);
}

enum TileType
{
    TILE_AIR,
    TILE_OBSTACLE,
    TILE_LILYPAD,
    TILE_FROG,

    TILE_RESERVED
};

enum Jump
{
    JUMP_UP,
    JUMP_HOP,
    JUMP_LAUNCH,
    JUMP_GLIDE,
    JUMP_STAY
};

struct Tile
{
    TileType type = TILE_AIR;
    void *next = nullptr;
};

Tile tiles[MAP_X * MAP_Y * MAP_Z];

struct Frog
{
    int position[3];
    Direction orientation;
    
    Jump jump = JUMP_STAY;
    int turning = 0;
    bool jumpCanceled = false;

    eng::Mat4f staticModelMatrix(float timePassed)
    {
        const float BREATHING = 0.025;
        eng::Vec3f modelPosition(2.0f * position[0], 2.0f * position[1], 2.0f * position[2]);
        return eng::Mat4f::translation(modelPosition.data)
             * eng::Mat4f::yRotation((M_PI / 2) * orientation)
             * eng::Mat4f::scale(1.0f, 1.0f - BREATHING + BREATHING * sin(timePassed * 2), 1.0f);
    }

    
};

struct LilyPad
{
    int position[3];
    Direction orientation;
    Frog *frog = nullptr;

    Frog *reservant = nullptr;

    bool turning = false;
    bool turningLeft;

    eng::Mat4f staticModelMatrix()
    {
        eng::Vec3f modelPosition(2.0f * position[0], 2.0f * position[1], 2.0f * position[2]);
        return eng::Mat4f::translation(modelPosition.data)
             * eng::Mat4f::yRotation((M_PI / 2) * orientation);
    }
};

std::vector<eng::Vector<3, int>> rocks;
std::vector<LilyPad*> lilyPads;
std::vector<Frog*> frogs;

void addRock(int x, int y, int z)
{
    rocks.push_back({ x, y, z });
    tiles[getTileIndex(x, y, z)] = { TILE_OBSTACLE };
}

void addLilyPad(int x, int y, int z, Direction dir)
{
    LilyPad *newLilyPad = new LilyPad { { x, y, z }, dir };
    lilyPads.push_back(newLilyPad);
    tiles[getTileIndex(x, y, z)] = { TILE_LILYPAD, newLilyPad };
}

void addFrog(int x, int y, int z, Direction dir)
{
    Frog *newFrog = new Frog { { x, y, z }, dir };
    frogs.push_back(newFrog);

    int tileIndex = getTileIndex(x, y, z);

    if (tiles[tileIndex].type == TILE_LILYPAD)
    {
        ((LilyPad*)(tiles[tileIndex].next))->frog = newFrog;
    }
    else
    {
        tiles[tileIndex] = { TILE_FROG, newFrog };
    }
}

std::vector<Tile*> reservedTiles;

void administerInput(Instruction instruction)
{
    if (instruction == INS_HOP || instruction == INS_LEAP)
    {
        for (Frog *frog : frogs)
        {
            int x = frog->position[0], z = frog->position[2];
            int y = frog->position[1];

            Tile *tTile;

            eng::Vector<2, int> front = moveFromDirection(frog->orientation);

            if (instruction == INS_HOP)
            {
                if ((tTile = &(tiles[getTileIndex(x + front[0], y, z + front[1])]))->type == TILE_AIR)
                {
                    tTile->type = TILE_RESERVED;
                    tTile->next = frog;
                    frog->jump = JUMP_HOP;
                    continue;
                }
                else if (tTyle->type == TILE_RESERVED)
                {
                    ((Frog*)(tTyle->next))->jumpCanceled = true;
                    frog->jump = JUMP_HOP;
                    frog->jumpCanceled = true;
                    continue;
                }
                else if (tTile->type == TILE_LILYPAD)
                {
                    LilyPad *lilyPad = (LilyPad*)(&(tTile->next));

                    if (lilyPad->frog == nullptr)
                    {
                        if (lilyPad->reservant == nullptr)
                        {
                            lilyPad->reservant = frog;
                            frog->jump = JUMP_HOP;
                        }
                        else
                        {
                            lilypad->reservant->jumpCanceled = true;
                            frog->jump = JUMP_HOP;
                            frog->jumpCanceled = true;
                        }

                        continue;
                    }
                }

                frog->jump = JUMP_STAY;
            }
            else
            {
                if ((tTile = &(tiles[getTileIndex(x + front[0], y + 1, z + front[1])]))->type == TILE_AIR || tTiles->type == TILE_RESERVED)
                {
                    if ((tTile = &(tiles[getTileIndex(x + front[0], y, z + front[1])]))->type == TILE_AIR)
                    {
                        if ((tTile = &(tiles[getTileIndex(x, y + 1, z)]))->type == TILE_AIR || tTile->type == TILE_RESERVED)
                        {
                            // Maybe cancelled (leap)
                            
                        }
                        else
                        {
                            // Bonked head (hop)
                        }
                    }
                    else if(tTile->type == TILE_RESERVED)
                    {
                        if ((tTile = &(tiles[getTileIndex(x, y + 1, z)]))->type == TILE_AIR || tTile->type == TILE_RESERVED)
                        {
                            // Maybe cancelled (leap)
                        }
                        else
                        {
                            // Bonked head (canceled hop)
                        }
                    }
                    else 
                    {
                        if (tTile->type == TILE_LILYPAD)
                        {
                            LilyPad *lilyPad = (LilyPad*)(&(tTile->next));

                            if (lilyPad->frog == nullptr)
                            {
                                if (lilyPad->reservant == nullptr)
                                {
                                    if ((tTile = &(tiles[getTileIndex(x, y + 1, z)]))->type == TILE_AIR || tTile->type == TILE_RESERVED)
                                    {
                                        // Maybe cancelled (leap)
                                    }
                                    else
                                    {
                                        // Bonked head (hop, into lily pad)
                                    }
                                }
                                else
                                {
                                    if ((tTile = &(tiles[getTileIndex(x, y + 1, z)]))->type == TILE_AIR || tTile->type == TILE_RESERVED)
                                    {
                                        // Maybe cancelled (leap)
                                    }
                                    else
                                    {
                                        // Bonked head (canceled hop, into lily pad)
                                    }
                                }

                                continue;
                            }
                        }

                        if ((tTile = &(tiles[getTileIndex(x, y + 1, z)]))->type == TILE_AIR)
                        {
                            // Reserve (up)
                        }
                        else if (tTile->type == TILE_RESERVED)
                        {
                            // (canceled up)
                        }
                        else
                        {
                            // (stay)
                        }
                    }
                }
                else
                {
                    // Kinda like hop but with end jump (up)
                }
            }
        }
    }
    else
    {
        int turnDirection = instruction == INS_TURN_LEFT ? 1 :-1;

        for (Frog *frog : frogs)
        {
            frog->turning += turnDirection;
            int x = frog->position[0], z = frog->position[2];
            int y = frog->position[1];

            if (tiles[getTileIndex(x, y, z)].type == TILE_LILYPAD)
            {
                continue;
            }

            Tile *currentTile;

            while (y < MAP_Y && (currentTile = &(tiles[getTileIndex(x, y, z)]))->type == TILE_FROG)
            {
                ((Frog *)(currentTile->next))->turning += turnDirection;
                ++y;
            }
        }
    }
}

void updateState()
{
    // change positions and clean up data
}




int main(int argc, char *argv[])
{
    addRock(1, 0, 1);
    addFrog(1, 1, 1, DIR_WEST);
    addLilyPad(-2, 1, 1, DIR_SOUTH);
    addFrog(-2, 1, 1, DIR_EAST);


    Commander::addCommand("kek", [] (int c, char *v[])
    {
        for (int i = 0; i < c; i++)
        {
            std::cout << v[i] << '\n';
            Console::write(v[i]);
        }
    });

    Application::setInitFlag(SoundEngine::FLAGS);
    Application::init();
    Application::setKeyCallback(&keyCallback);
    SoundEngine::init();

    SDL_StartTextInput();
//     SDL_StopTextInput();

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Sound stuff

    SoundEngine::Track noiceWave = SoundEngine::load("../cringe/noice.wav");

    SoundEngine::ClipInfo noiceInfo
    {
        noiceWave.buffer,
        0, noiceWave.length,
        0, SE_STATIC_VOLUME | SE_STATIC_SPEED,
        1.0f, 1.0f, 1.0f,
        0.0f
    };

    SoundEngine::Clip noice{noiceInfo};

    Commander::addCommand("noice", [&noice] (int c, char *v[])
    {
        noice.play();
    });

    Commander::addCommand("hell", [&noiceInfo] (int c, char *v[])
    {
        const u32 OFFSET = 512;
        const u32 COUNT  = 16;

        noiceInfo.flags = 0;
        noiceInfo.volumeL = (1.0f / COUNT) * 8;
        noiceInfo.volumeR = (1.0f / COUNT) * 8 * 0;
        noiceInfo.speed = 0.25f;

        for (int i = 0; i < COUNT; i++)
        {
            noiceInfo.progress = OFFSET * i;
            SoundEngine::dispatch(noiceInfo);
        }

        noiceInfo.flags = SE_STATIC_VOLUME | SE_STATIC_SPEED;
        noiceInfo.volumeL = 1.0f;
        noiceInfo.volumeR = 1.0f;
        noiceInfo.speed = 1.0f;
        noiceInfo.progress = 0;
    });

// Asset loading

    loadAssets();

// Vao stuff

    glGenVertexArrays(1, &VAO(CUBE));
    glBindVertexArray(VAO(CUBE));

    glGenBuffers(1, &BUF(CUBE_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(CUBE_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(Models::cubeVertices), Models::cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &BUF(CUBE_IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUF(CUBE_IBO));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Models::cubeIndices), Models::cubeIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

// Texture stuff

// Shader stuff

    Shader::generate(
        PRG(CLASSIC),
        SHD(CLASSIC_VERTEX), SHD(CLASSIC_FRAGMENT),
        Shader::load("shaders/shader.vert"),
        Shader::load("shaders/shader.frag")
    );

    int mvpMatrix = glGetUniformLocation(PRG(CLASSIC), "u_mvpMat");
    int modMatrix = glGetUniformLocation(PRG(CLASSIC), "u_modMat");
    int camPos    = glGetUniformLocation(PRG(CLASSIC), "u_cameraPos");
    int objColor  = glGetUniformLocation(PRG(CLASSIC), "u_objColor");

    glUseProgram(PRG(CLASSIC));
    glUniform1i(glGetUniformLocation(PRG(CLASSIC), "textureSampler"), 0);

    Shader::generate(
        PRG(PLAIN),
        SHD(PLAIN_VERTEX), SHD(PLAIN_FRAGMENT),
        Shader::load("shaders/plain.vert"),
        Shader::load("shaders/plain.frag")
    );

    int plainMvpMatrix = glGetUniformLocation(PRG(PLAIN), "u_mvpMat");
    int plainModMatrix = glGetUniformLocation(PRG(PLAIN), "u_modMat");
    int plainCamPos    = glGetUniformLocation(PRG(PLAIN), "u_cameraPos");
    int plainObjColor  = glGetUniformLocation(PRG(PLAIN), "u_objColor");

// Init other stuff

    Text::init();
    EZ2D::init();

// Doo doo

    eng::Vec3f cameraPos(0.f, 0.f, 3.f);
    eng::Vec2f cameraRot(0.f, 0.f);
    eng::Vec3f objPos(-1.0f, 2.0f,-0.5);

    auto tp1 = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;
    double timePassed = 0.0;

    int wWidth, wHeight;
    SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

    Application::setMouseMotionCallback([&](SDL_Event &event)
    {
        if (cursorEnabled) return;

        cameraRot[0] += (float)(event.motion.yrel) / 256.f;
        cameraRot[1] += (float)(event.motion.xrel) / 256.f;
    });

    glActiveTexture(GL_TEXTURE0);

    while (Application::running)
    {
        Application::pollEvents();

        float speed = 15.0f * deltaTime;
        float cosVel = cos(cameraRot[1]) * speed;
        float sinVel = sin(cameraRot[1]) * speed;

        if (KEY(W))
        {
            cameraPos[2] -= cosVel;
            cameraPos[0] += sinVel;
        }

        if (KEY(A))
        {
            cameraPos[2] -= sinVel;
            cameraPos[0] -= cosVel;
        }

        if (KEY(S))
        {
            cameraPos[2] += cosVel;
            cameraPos[0] -= sinVel;
        }

        if (KEY(D))
        {
            cameraPos[2] += sinVel;
            cameraPos[0] += cosVel;
        }

        if (KEY(SPACE))
        {
            cameraPos[1] += speed;
        }

        if (KEY(SHIFT))
        {
            cameraPos[1] -= speed;
        }

//         if (KEY(UP) || KEY(K))
//         {
//             cameraRot[0] -= speed * .2f;
//         }
// 
//         if (KEY(DOWN) || KEY(J))
//         {
//             cameraRot[0] += speed * .2f;
//         }
// 
//         if (KEY(LEFT) || KEY(H))
//         {
//             cameraRot[1] -= speed * .2f;
//         }
// 
//         if (KEY(RIGHT) || KEY(L))
//         {
//             cameraRot[1] += speed * .2f;
//         }

        SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

        eng::Mat4f projMat = eng::Mat4f::GL_Projection(90.f, wWidth, wHeight, 0.1f, 100.f);
        eng::Mat4f viewMat = eng::Mat4f::xRotation(cameraRot[0])
            * eng::Mat4f::yRotation(cameraRot[1])
            * eng::Mat4f::translation((-cameraPos).data);

        eng::Mat4f mvpMat = projMat * viewMat;

        glClearColor(0.7f, 0.8f, 0.95f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        eng::Mat4f groundTransform = eng::Mat4f::translation(0.0f, -1.0f, 0.0f)
                                   * eng::Mat4f::scale(20.0f, 1.0f, 20.0f);

        glBindVertexArray(VAO(CUBE));

        glUseProgram(PRG(PLAIN));

        glUniformMatrix4fv(plainMvpMatrix, 1, false, mvpMat.data);
        glUniformMatrix4fv(plainModMatrix, 1, false, groundTransform.data);
        glUniform3fv(plainCamPos, 1, cameraPos.data);
        glUniform3f(plainObjColor, 0.8f, 0.7f, 0.45f);

        glDrawElements(GL_TRIANGLES, sizeof(Models::cubeIndices) / sizeof(u32), GL_UNSIGNED_INT, (void*)(0));


        glUseProgram(PRG(CLASSIC));
        glUniformMatrix4fv(mvpMatrix, 1, false, mvpMat.data);
        glUniform3fv(camPos, 1, cameraPos.data);
        glUniform3f(objColor, 1.0f, 1.0f, 1.0f);

        glBindTexture(GL_TEXTURE_2D, assets[ASS_ROCK].texture);
        glBindVertexArray(assets[ASS_ROCK].vao);

        for (auto &rock : rocks)
        {
            eng::Vec3f modelPosition(2.0f * rock[0], 2.0f * rock[1], 2.0f * rock[2]);
            eng::Mat4f modelMatrix = eng::Mat4f::translation(modelPosition.data);

            glUniformMatrix4fv(modMatrix, 1, false, modelMatrix.data);
    
            glDrawElements(GL_TRIANGLES, assets[ASS_ROCK].indexCount, GL_UNSIGNED_INT, (void*)(0));
        }

        glBindTexture(GL_TEXTURE_2D, assets[ASS_LILYPAD].texture);
        glBindVertexArray(assets[ASS_LILYPAD].vao);

        for (LilyPad *lilyPad : lilyPads)
        {
            glUniformMatrix4fv(modMatrix, 1, false, lilyPad->staticModelMatrix().data);
    
            glDrawElements(GL_TRIANGLES, assets[ASS_LILYPAD].indexCount, GL_UNSIGNED_INT, (void*)(0));
        }

        glBindTexture(GL_TEXTURE_2D, assets[ASS_FROG].texture);
        glBindVertexArray(assets[ASS_FROG].vao);

        for (Frog *frog : frogs)
        {
            glUniformMatrix4fv(modMatrix, 1, false, frog->staticModelMatrix(timePassed).data);
    
            glDrawElements(GL_TRIANGLES, assets[ASS_FROG].indexCount, GL_UNSIGNED_INT, (void*)(0));
        }

        glBindTexture(GL_TEXTURE_2D, assets[ASS_SHADOW].texture);
        glBindVertexArray(assets[ASS_SHADOW].vao);

        for (Frog *frog : frogs)
        {
            auto &pos = frog->position;

            bool isOnLilyPad = tiles[getTileIndex(pos[0], pos[1], pos[2])].type == TILE_LILYPAD;
            float yOffset = 0.001f + (isOnLilyPad ? (1.0f / 8) : 0.0f);

            eng::Vec3f modelPosition(2.0f * pos[0], 2.0f * pos[1] + yOffset, 2.0f * pos[2]);
            eng::Mat4f modelMatrix = eng::Mat4f::translation(modelPosition.data)
                                   * eng::Mat4f::scale(0.8, 1.0, 0.8);

            glUniformMatrix4fv(modMatrix, 1, false, modelMatrix.data);
    
            glDrawElements(GL_TRIANGLES, assets[ASS_SHADOW].indexCount, GL_UNSIGNED_INT, (void*)(0));
        }


        glDisable(GL_DEPTH_TEST);

        if (Console::active)
        {
            Console::render();
        }

        glEnable(GL_DEPTH_TEST);


        SDL_GL_SwapWindow(Application::window);

        auto tp2 = std::chrono::high_resolution_clock::now();
        deltaTime = (float)((tp2 - tp1).count()) / 1000000000.f;
        timePassed += deltaTime;
        tp1 = tp2;
    }

    for (LilyPad *lilyPad : lilyPads)
    {
        delete lilyPad;
    }

    for (Frog *frog : frogs)
    {
        delete frog;
    }

    Shader::destroy(PRG(CLASSIC), SHD(CLASSIC_VERTEX), SHD(CLASSIC_FRAGMENT));
    Shader::destroy(PRG(PLAIN), SHD(PLAIN_VERTEX), SHD(PLAIN_FRAGMENT));

    freeAssets();

    glDeleteBuffers(1, &BUF(CUBE_VBO));
    glDeleteBuffers(1, &BUF(CUBE_IBO));
    glDeleteVertexArrays(1, &VAO(CUBE));

    Text::close();
    EZ2D::close();

    SoundEngine::close();
    Application::close();

    return 0;
}

