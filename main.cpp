#include "application.hpp"

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <stb_image.h>

#include <iostream>
#include <string>
#include <chrono>

#include <cmath>

#include "math.hpp"
#include "objmf.hpp"
#include "shaders.hpp"
#include "textures.hpp"

float cubeVertices[] = {
     1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
     1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
    -1.0f,-1.0f, 1.0f,  0.0f, 0.0f, 1.0f,

    -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,-1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f, 1.0f,-1.0f, -1.0f, 0.0f, 0.0f,
    -1.0f,-1.0f,-1.0f, -1.0f, 0.0f, 0.0f,

    -1.0f, 1.0f,-1.0f,  0.0f, 0.0f,-1.0f,
    -1.0f,-1.0f,-1.0f,  0.0f, 0.0f,-1.0f,
     1.0f, 1.0f,-1.0f,  0.0f, 0.0f,-1.0f,
     1.0f,-1.0f,-1.0f,  0.0f, 0.0f,-1.0f,

     1.0f, 1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,-1.0f,-1.0f,  1.0f, 0.0f, 0.0f,
     1.0f, 1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
     1.0f,-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,

     1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
     1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,
    -1.0f, 1.0f,-1.0f,  0.0f, 1.0f, 0.0f,

     1.0f,-1.0f,-1.0f,  0.0f,-1.0f, 0.0f,
    -1.0f,-1.0f,-1.0f,  0.0f,-1.0f, 0.0f,
     1.0f,-1.0f, 1.0f,  0.0f,-1.0f, 0.0f,
    -1.0f,-1.0f, 1.0f,  0.0f,-1.0f, 0.0f
};
uint32_t cubeIndices[] = {
    0, 2, 3,  0, 3, 1,
    4, 6, 7,  4, 7, 5,
    8, 10, 11,  8, 11, 9,
    12, 14, 15,  12, 15, 13,
    16, 18, 19,  16, 19, 17,
    20, 22, 23,  20, 23, 21
};

inline int wrapAround(int x, int y) {
    int a = abs(x) % y;
    if (x < 0) {
        return y - a;
    }
    return a;
}

struct TexCoordInfo {
    float ax, ay, bx, by;
    float cx, cy, dx, dy;
    float ex, ey, fx, fy;
    float sm;
};

constexpr uint32_t nCillinderVC(uint32_t n, bool texCoords = false) { return (texCoords ? 48 : 36) * n; }
constexpr uint32_t nCillinderIC(uint32_t n) { return 12 * n - 12; }
void nCillinder(
    int n,
    float *vertices,
    uint32_t *indices,
    const TexCoordInfo *texInfo = nullptr
) {
    if (n < 1) {
        std::cerr << "Error: Can't create a cillinder with '" << n << "' sides.\n";
    }

    bool texturing = texInfo != nullptr;
    int vSize = (texturing ? 8 : 6);
    float cx[2], cy[2], lx[2], ly[2], ud[2];
    float seg = 0.f, span = 0.f;
    if (texturing) {
        cx[0] = (texInfo->bx + texInfo->ax) * .5f; 
        cx[1] = (texInfo->dx + texInfo->cx) * .5f;
        cy[0] = (texInfo->by + texInfo->ay) * .5f;
        cy[1] = (texInfo->dy + texInfo->cy) * .5f;
        lx[0] = (texInfo->bx - texInfo->ax) * .5f;
        lx[1] = (texInfo->dx - texInfo->cx) * .5f;
        ly[0] = (texInfo->by - texInfo->ay) * .5f;
        ly[1] = (texInfo->dy - texInfo->cy) * .5f;
        ud[0] = texInfo->ey;
        ud[1] = texInfo->fy;
        span = texInfo->fx - texInfo->ex;
        seg = (span / (float)n) * texInfo->sm;
    }

    float angle = (M_PI * 2) / n;
    for (int i = 0; i < n; i++) {
        float x = sin(angle * i), z = cos(angle * i);
        const float y[] = { 1,-1 };
        for (int j = 0; j < 2; j++) {
            // Tops
            int offset = n * vSize * j + vSize * i;

            vertices[offset]     = x;
            vertices[offset + 1] = y[j];
            vertices[offset + 2] = z;

            vertices[offset + 3] = 0.0f;
            vertices[offset + 4] = y[j];
            vertices[offset + 5] = 0.0f;

            if (texturing) {
                vertices[offset + 6] = cx[j] + x * lx[j];
                vertices[offset + 7] = cy[j] + z * ly[j];
            }

            // Sides
            int offset1 = n * vSize * 2 + vSize * j + i * vSize * 4;
            int offset2 = n * vSize * 2 + vSize * j + wrapAround(i * 2 - 1, n * 2) * vSize * 2;

            vertices[offset1]     = x;
            vertices[offset1 + 1] = y[j];
            vertices[offset1 + 2] = z;

            vertices[offset1 + 3] = sin(angle * i + (angle / 2));
            vertices[offset1 + 4] = 0.0f;
            vertices[offset1 + 5] = cos(angle * i + (angle / 2));

            vertices[offset2]     = x;
            vertices[offset2 + 1] = y[j];
            vertices[offset2 + 2] = z;

            vertices[offset2 + 3] = sin(angle * i - (angle / 2));
            vertices[offset2 + 4] = 0.0f;
            vertices[offset2 + 5] = cos(angle * i - (angle / 2));

            if (texturing) {
                float segOffset = texInfo->ex + fmod(seg * i, span);
                vertices[offset1 + 6] = segOffset;
                vertices[offset1 + 7] = ud[j];
                vertices[offset2 + 6] = i == 0 ? texInfo->fx : segOffset;
                vertices[offset2 + 7] = ud[j];
            }
        }
    }

    int i = 0;
    for (uint32_t v = 1; v < n - 1; v++) {
        indices[i]     = 0;
        indices[i + 1] = v;
        indices[i + 2] = v + 1;
        i += 3;
    }
    for (uint32_t v = n * 2 - 1; v > n + 1; v--) {
        indices[i]     = n;
        indices[i + 1] = v;
        indices[i + 2] = v - 1;
        i += 3;
    }
    for (uint32_t j = 0; j < n; j++) {
        uint32_t v = 2 * n + 4 * j;
        indices[i]     = v;
        indices[i + 1] = v + 3;
        indices[i + 2] = v + 2;
        indices[i + 3] = v;
        indices[i + 4] = v + 1;
        indices[i + 5] = v + 3;
        i += 6;
    }
}

float rectangleVertices[] = {
     1.0f,-1.0f,  1.0f, 1.0f,
     1.0f, 0.0f,  1.0f, 0.0f,
     0.0f, 0.0f,  0.0f, 0.0f,

     0.0f,-1.0f,  0.0f, 1.0f,
     1.0f,-1.0f,  1.0f, 1.0f,
     0.0f, 0.0f,  0.0f, 0.0f
};

const int STRING_BUFFER_LENGTH = 2048;

uint8_t charBuffer[STRING_BUFFER_LENGTH] = { 0 };

void bufferChars(const char *str, int offset, int count) {
    for (int i = 0; i < count; i++) {
        uint8_t c = (uint8_t)(str[i]);
        if (c == 0) {
            break;
        }
        charBuffer[offset + i] = c;
    }
}

enum KeySigns {
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

const uint32_t ScanCodes[] {
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

uint32_t windowType = 0;
#ifdef _WIN32
const uint32_t FULLSCREEN = SDL_WINDOW_FULLSCREEN;
#else
const uint32_t FULLSCREEN = SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

bool cursorEnabled = false;

void keyCallback(SDL_Event &event, bool down) {
    SDL_Scancode scancode = event.key.keysym.scancode;
    for (int i = 0; i < KEY_COUNT; i++) {
        if (scancode == ScanCodes[i]) {
            keyStates[i] = down;
            return;
        }
    }
    if (!down) return;
    switch (scancode) {

        case SDL_SCANCODE_F11:
            if (windowType == 0) {
                windowType = FULLSCREEN;
            } else {
                windowType = 0;
            }
            if (SDL_SetWindowFullscreen(Application::window, windowType) != 0) {
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

        default : return;
    }
}

int main(int argc, char *argv[]) {
    const int POINT_COUNT = 32;
    const TexCoordInfo texInfo {
        .0f, .5f, .5f, 1.f,
        .5f, .5f, 1.f, 1.f,
        .0f, .0f, 1.f, .5f,
        4.f
    };
    float *verts = new float[nCillinderVC(POINT_COUNT, true)];
    uint32_t *inds = new uint32_t[nCillinderIC(POINT_COUNT)];
    nCillinder(POINT_COUNT, verts, inds, &texInfo);

    const char *boi = "abcdefghijklmnopqrtsuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    bufferChars(boi, 0, strlen(boi));

    Application::init();
    Application::setKeyCallback(keyCallback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
//     glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

// Vao stuff

    uint32_t vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    uint32_t vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, nCillinderVC(POINT_COUNT, true) * sizeof(float), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    uint32_t ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nCillinderIC(POINT_COUNT) * sizeof(uint32_t), inds, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] inds;
    delete[] verts;


    uint32_t rectVao;
    glGenVertexArrays(1, &rectVao);
    glBindVertexArray(rectVao);

    uint32_t rectVbo;
    glGenBuffers(1, &rectVbo);
    glBindBuffer(GL_ARRAY_BUFFER, rectVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    uint32_t textVao;
    glGenVertexArrays(1, &textVao);
    glBindVertexArray(textVao);

    uint32_t textVbo;
    glGenBuffers(1, &textVbo);
    glBindBuffer(GL_ARRAY_BUFFER, textVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    uint32_t stringBuffer;
    glGenBuffers(1, &stringBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, stringBuffer);
    glBufferData(GL_ARRAY_BUFFER, STRING_BUFFER_LENGTH, charBuffer, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(uint8_t), (void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

// Texture stuff

    int width, height, channelCount;
    stbi_set_flip_vertically_on_load(false);
    uint8_t *data = stbi_load("../cringe/kringo.png", &width, &height, &channelCount, STBI_rgb_alpha);
    if (!data) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    TexParameters parameters = {
        data,
        width, height,
        GL_REPEAT, GL_REPEAT,
        GL_NEAREST_MIPMAP_NEAREST, GL_NEAREST,
        GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE
    };

    Texture texture(parameters);

    free(data);

    int widthC, heightC, channelCountC;
    uint8_t *img = stbi_load("../cringe/charsheet.png", &widthC, &heightC, &channelCountC, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    parameters.minFilter = GL_NEAREST;
    parameters.data = img;
    parameters.width = widthC;
    parameters.height = heightC;
    parameters.deviceFormat = GL_RED;

    Texture charSheet(parameters, false);

    free(img);


// Shader stuff

    GraphicShader shader(
        GraphicShader::load("shaders/shader.vert"),
        GraphicShader::load("shaders/shader.frag")
    );

    int mvpMatrix = shader.getUniform("u_mvpMat");
    int modMatrix = shader.getUniform("u_modMat");
    int camPos = shader.getUniform("u_cameraPos");
    int objColor = shader.getUniform("u_objColor");

    shader.use();
    glUniform1i(shader.getUniform("textureSampler"), 0);

    GraphicShader rectShader(
        GraphicShader::load("shaders/rect.vert"),
        GraphicShader::load("shaders/rect.frag")
    );

    int rPosition = rectShader.getUniform("u_position");
    int rScale = rectShader.getUniform("u_scale");

    rectShader.use();
    glUniform1i(rectShader.getUniform("u_sampler"), 0);

    GraphicShader textShader(
        GraphicShader::load("shaders/text.vert"),
        GraphicShader::load("shaders/text.frag")
    );

    int textTrans = textShader.getUniform("u_transform");
    int textColor = textShader.getUniform("u_textColor");

    textShader.use();
    glUniform1i(textShader.getUniform("u_charSheet"), 1);
    GraphicShader::useNone();

// Doo doo

    eng::Vec3f cameraPos(0.f, 0.f, 3.f);
    eng::Vec2f cameraRot(0.f, 0.f);
    eng::Vec3f objPos(-1.0f, 2.0f,-0.5);

    eng::Vec2f rPos(-0.5f, 0.5f);
    eng::Vec2f rScl(0.5f, 0.5f);

    auto tp1 = std::chrono::high_resolution_clock::now();
    float deltaTime = 0.0f;
    double timePassed = 0.0;

    int wWidth, wHeight;
    SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

    Application::setMouseMotionCallback([&](SDL_Event &event) {
        if (cursorEnabled) return;
        cameraRot[0] += (float)(event.motion.yrel) / 256.f;
        cameraRot[1] += (float)(event.motion.xrel) / 256.f;
    });

    while (Application::running) {
        Application::pollEvents();

        float speed = 15.0f * deltaTime;
        float cosVel = cos(cameraRot[1]) * speed;
        float sinVel = sin(cameraRot[1]) * speed;

        if (keyStates[KEY_W]) {
            cameraPos[2] -= cosVel;
            cameraPos[0] += sinVel;
        }
        if (keyStates[KEY_A]) {
            cameraPos[2] -= sinVel;
            cameraPos[0] -= cosVel;
        }
        if (keyStates[KEY_S]) {
            cameraPos[2] += cosVel;
            cameraPos[0] -= sinVel;
        }
        if (keyStates[KEY_D]) {
            cameraPos[2] += sinVel;
            cameraPos[0] += cosVel;
        }
        if (keyStates[KEY_SPACE]) {
            cameraPos[1] += speed;
        }
        if (keyStates[KEY_SHIFT]) {
            cameraPos[1] -= speed;
        }
        if (keyStates[KEY_UP] || keyStates[KEY_K]) {
            cameraRot[0] -= speed * .2f;
        }
        if (keyStates[KEY_DOWN] || keyStates[KEY_J]) {
            cameraRot[0] += speed * .2f;
        }
        if (keyStates[KEY_LEFT] || keyStates[KEY_H]) {
            cameraRot[1] -= speed * .2f;
        }
        if (keyStates[KEY_RIGHT] || keyStates[KEY_L]) {
            cameraRot[1] += speed * .2f;
        }

        eng::Vec3f axis = eng::Vec3f(1.0f, 1.0f, 1.0f) * sin(timePassed * 0.8f);
        eng::Quaternionf roter(cos(timePassed * 0.8f), axis.data);
        auto rotPos = roter.conjugate() * eng::Quaternionf(0.0f, objPos.data) * roter;

        SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

        eng::Mat4f modeMat = eng::Mat4f::translation(rotPos.i, rotPos.j, rotPos.k)
            * eng::Mat4f::rotation(roter.normalize());
//         eng::Mat4f cringeMat(true);
        eng::Mat4f projMat = eng::Mat4f::GL_Projection(90.f, wWidth, wHeight, 0.1f, 100.f);
        eng::Mat4f viewMat = eng::Mat4f::xRotation(cameraRot[0])
            * eng::Mat4f::yRotation(cameraRot[1])
            * eng::Mat4f::translation((-cameraPos).data);

        eng::Mat4f mvpMat = projMat * viewMat;

        glClearColor(0.35f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture.bind(0, true);

        glBindVertexArray(vao);

        shader.use();

        glUniformMatrix4fv(mvpMatrix, 1, false, mvpMat.data);
        glUniformMatrix4fv(modMatrix, 1, false, modeMat.data);
//         glUniformMatrix4fv(modMatrix, 1, false, cringeMat.data);
        glUniform3fv(camPos, 1, cameraPos.data);
        glUniform3f(objColor, 0.8f, 0.7f, 0.45f);

        glDrawElements(GL_TRIANGLES, nCillinderIC(POINT_COUNT), GL_UNSIGNED_INT, (void*)(0));


        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(textVao);
        textShader.use();

        charSheet.bind(1, true);

        glUniform4f(textTrans,-0.9f,-0.5f, 8.f / 540.f, 16.f / 360.f);
        glUniform4f(textColor, 1.0f, 1.0f, 1.0f, 1.0f);

        glDrawArraysInstancedBaseInstance(GL_TRIANGLES, 0, 6, strlen(boi), 0);


        glBindVertexArray(rectVao);
        rectShader.use();
        texture.bind(0, true);

        glUniform2fv(rPosition, 1, rPos.data);
        glUniform2fv(rScale, 1, rScl.data);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glEnable(GL_DEPTH_TEST);


        SDL_GL_SwapWindow(Application::window);

        auto tp2 = std::chrono::high_resolution_clock::now();
        deltaTime = (float)((tp2 - tp1).count()) / 1000000000.f;
        timePassed += deltaTime;
        tp1 = tp2;
    }

    shader.free();
    rectShader.free();
    textShader.free();

    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    glDeleteVertexArrays(1, &vao);

    glDeleteBuffers(1, &rectVbo);
    glDeleteVertexArrays(1, &rectVao);

    glDeleteBuffers(1, &textVbo);
    glDeleteBuffers(1, &stringBuffer);
    glDeleteVertexArrays(1, &textVao);

    Application::close();

    return 0;
}

