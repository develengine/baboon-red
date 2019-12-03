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
#include "oglcore.hpp"

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



namespace eng {

int ustrlen(const uint8_t *str)
{
    int i = 0;
    while (str[i] != 0) {
        ++i;
    }
    return i;
}

void strToUStr(
    const char *str,
    uint8_t *dest,
    const  uint8_t *subs = nullptr,
    char del = '%'
) {
    int subIndex = 0;
    int i = 0;
    char c = 0;
    if (subs == nullptr) {
        while ((c = str[i]) != 0) {
            dest[i] = (uint8_t)c;
            i++;
        }
    } else {
        while ((c = str[i]) != 0) {
            if (c == del) {
                dest[i] = subs[subIndex++];
            } else {
                dest[i] = (uint8_t)c;
            }
            i++;
        }
    }
    dest[i] = 0;
}

template<typename T, uint32_t N>
class WrapStack {
public:
    int dataPointer = 0;
    int pushCounter = 0;
    T data[N];

    inline int size() const
    {
        return pushCounter < N ? pushCounter : N;
    }

    inline int position(int index) const
    {
        return (N + ((dataPointer - index) % N)) % N;
    }

    void push(const T &e)
    {
        dataPointer = (dataPointer + 1) % N;
        data[dataPointer] = e;
        ++pushCounter;
    }

    T& get()
    {
        dataPointer = (dataPointer + 1) % N;
        ++pushCounter;
        return data[dataPointer];
    }

    void clear()
    {
        dataPointer = 0;
        pushCounter = 0;
    }

    T& operator[] (int index)
    {
        return data[position(index)];
    }

    const T& operator[] (int index) const
    {
        return data[position(index)];
    }
};

}

namespace Text {

float textRectangleVertices[] = {
     1.0f,-1.0f,  1.0f, 1.0f,
     1.0f, 0.0f,  1.0f, 0.0f,
     0.0f, 0.0f,  0.0f, 0.0f,

     0.0f,-1.0f,  0.0f, 1.0f,
     1.0f,-1.0f,  1.0f, 1.0f,
     0.0f, 0.0f,  0.0f, 0.0f
};

const int LINE_LENGTH = 128;
const int LINE_COUNT  = 64;
const int TEXT_MEMORY_SIZE = 1028 * 32;
const int DEVICE_MEMORY_OFFSET = 0;

eng::Vec2f consolePosition(-1.0f, 1.0f);
int lineOffset = 0;

struct ConsoleLine {
    uint8_t characters[LINE_LENGTH];
};

eng::WrapStack<ConsoleLine, LINE_COUNT> lineBuffer;
eng::WrapStack<int, LINE_COUNT> lineLengths;

int newLineCount = 0;

// TODO [console]: string / const char writes

void consoleWrite(const uint8_t *text)
{
    int length = eng::ustrlen(text);
    int fullCount = length / LINE_LENGTH;
    int leftover = length % LINE_LENGTH;

    ConsoleLine *line;
    if (leftover > 0) {
        line = &(lineBuffer.get());
        memcpy(line->characters, &(text[fullCount * LINE_LENGTH]), leftover);
        lineLengths.push(leftover);
        ++newLineCount;
    }
    for (int i = fullCount - 1; i >= 0; i--) {
        line = &(lineBuffer.get());
        memcpy(line->characters, &(text[i * LINE_LENGTH]), LINE_LENGTH);
        lineLengths.push(LINE_LENGTH);
        ++newLineCount;
    }
}

void consoleRender()
{
    glBindVertexArray(VAO(TEXT));
    SHD(TEXT).use();
    
    // TODO @Performance: Merge glBufferSubData calls into one/two

    if (newLineCount > 0) {
        glBindBuffer(GL_ARRAY_BUFFER, BUF(TEXT_STRING));
        int cycles = std::min(LINE_COUNT, newLineCount);
        for (int i = 0; i < cycles; i++) {
            int localOffset = LINE_LENGTH * lineLengths.position(i);
            glBufferSubData(
                GL_ARRAY_BUFFER,
                DEVICE_MEMORY_OFFSET + localOffset, 
                lineLengths[i],
                lineBuffer[i].characters
            );
        }
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        newLineCount = 0;
    }

    glUniform1i(UNI(TEXT_WRAP), LINE_LENGTH);
    glUniform4f(UNI(TEXT_COLOR), 1.0f, 1.0f, 1.0f, 1.0f);

    // TODO [console]: color; line folding; scrolling

    int lineCount = std::min(32, lineLengths.size());
    for (int i = 0; i < lineCount; i++) {
        int wWidth, wHeight;
        SDL_GetWindowSize(Application::window, &wWidth, &wHeight);
        eng::Vec2f characterSize(8.f / (float)(wWidth / 2), 16.f / (float)(wHeight / 2));
        glUniform4f( UNI(TEXT_TRANSFORM),
            consolePosition[0], consolePosition[1] - characterSize[1] * i,
            characterSize[0], characterSize[1]
        );
        glDrawArraysInstancedBaseInstance( GL_TRIANGLES,
            0, 6,
            lineLengths[i + lineOffset],
            lineLengths.position(i + lineOffset) * LINE_LENGTH
        );
    }
}



void init()
{
    // Vao
    glGenVertexArrays(1, &VAO(TEXT));
    glBindVertexArray(VAO(TEXT));

    glGenBuffers(1, &BUF(TEXT_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(TEXT_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(textRectangleVertices), textRectangleVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &BUF(TEXT_STRING));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(TEXT_STRING));
    glBufferData(GL_ARRAY_BUFFER, TEXT_MEMORY_SIZE, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(uint8_t), (void*)0);
    glEnableVertexAttribArray(2);

    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);


    // Texture
    int width, height, channelCount;
    uint8_t *img = stbi_load("../cringe/charsheet.png", &width, &height, &channelCount, STBI_rgb_alpha);
    if (!img) {
        std::cerr << "Failed to load image\n";
        exit(-1);
    }

    TexParameters parameters = {
        img,
        width, height,
        GL_REPEAT, GL_REPEAT,
        GL_NEAREST, GL_NEAREST,
        GL_RGBA, GL_RED, GL_UNSIGNED_BYTE
    };

    TEX(CHARSHEET).generate(parameters, false);

    free(img);

    TEX(CHARSHEET).bind(1, true);


    // Shader
    SHD(TEXT).generate(
        GraphicShader::load("shaders/text.vert"),
        GraphicShader::load("shaders/text.frag")
    );

    UNI(TEXT_TRANSFORM) = SHD(TEXT).getUniform("u_transform");
    UNI(TEXT_COLOR) = SHD(TEXT).getUniform("u_textColor");
    UNI(TEXT_WRAP)  = SHD(TEXT).getUniform("u_wrapLen");

    SHD(TEXT).use();
    glUniform1i(SHD(TEXT).getUniform("u_charSheet"), 1);
    GraphicShader::useNone();
}

void exit()
{
    glDeleteBuffers(1, &BUF(TEXT_VBO));
    glDeleteBuffers(1, &BUF(TEXT_STRING));
    glDeleteVertexArrays(1, &VAO(TEXT));
}

}


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

const uint32_t ScanCodes[]
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

uint32_t windowType = 0;
#ifdef _WIN32
const uint32_t FULLSCREEN = SDL_WINDOW_FULLSCREEN;
#else
const uint32_t FULLSCREEN = SDL_WINDOW_FULLSCREEN_DESKTOP;
#endif

bool cursorEnabled = false;

void keyCallback(SDL_Event &event, bool down)
{
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

int main(int argc, char *argv[])
{
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

    const char boi[] = "abcdefghijklmnopqrstuvwxyz0123456789%%%";
    uint8_t reps[] = { 230, 231, 233 };
    uint8_t dest[sizeof(boi)];
    eng::strToUStr(boi, dest, reps);
    Text::consoleWrite(dest);
    Text::consoleWrite(dest);

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

    glGenVertexArrays(1, &VAO(CLASSIC));
    glBindVertexArray(VAO(CLASSIC));

    glGenBuffers(1, &BUF(CLASSIC_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(CLASSIC_VBO));
    glBufferData(GL_ARRAY_BUFFER, nCillinderVC(POINT_COUNT, true) * sizeof(float), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &BUF(CLASSIC_IBO));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, BUF(CLASSIC_IBO));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nCillinderIC(POINT_COUNT) * sizeof(uint32_t), inds, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    delete[] inds;
    delete[] verts;


    glGenVertexArrays(1, &VAO(RECTANGLE));
    glBindVertexArray(VAO(RECTANGLE));

    glGenBuffers(1, &BUF(RECTANGLE_VBO));
    glBindBuffer(GL_ARRAY_BUFFER, BUF(RECTANGLE_VBO));
    glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


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

    TEX(CLASSIC).generate(parameters);

    free(data);


// Shader stuff

    SHD(CLASSIC).generate(
        GraphicShader::load("shaders/shader.vert"),
        GraphicShader::load("shaders/shader.frag")
    );

    int mvpMatrix = SHD(CLASSIC).getUniform("u_mvpMat");
    int modMatrix = SHD(CLASSIC).getUniform("u_modMat");
    int camPos = SHD(CLASSIC).getUniform("u_cameraPos");
    int objColor = SHD(CLASSIC).getUniform("u_objColor");

    SHD(CLASSIC).use();
    glUniform1i(SHD(CLASSIC).getUniform("textureSampler"), 0);

    SHD(RECTANGLE).generate(
        GraphicShader::load("shaders/rect.vert"),
        GraphicShader::load("shaders/rect.frag")
    );

    int rPosition = SHD(RECTANGLE).getUniform("u_position");
    int rScale = SHD(RECTANGLE).getUniform("u_scale");

    SHD(RECTANGLE).use();
    glUniform1i(SHD(RECTANGLE).getUniform("u_sampler"), 0);

    SHD(RECT_COL).generate(
        GraphicShader::load("shaders/rectcolor.vert"),
        GraphicShader::load("shaders/rectcolor.frag")
    );

    int colPosition = SHD(RECT_COL).getUniform("u_position");
    int colScale = SHD(RECT_COL).getUniform("u_scale");
    int colColor = SHD(RECT_COL).getUniform("u_color");


    Text::init();

// Doo doo

    eng::Vec3f cameraPos(0.f, 0.f, 3.f);
    eng::Vec2f cameraRot(0.f, 0.f);
    eng::Vec3f objPos(-1.0f, 2.0f,-0.5);

    eng::Vec2f rPos(-0.5f, 0.5f);
    eng::Vec2f rScl(0.5f, 0.5f);

    eng::Vec2f consolePos(-1.0f, 0.75f);
    eng::Vec2f consoleScl(1.5f, 1.5f);

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

    TEX(CLASSIC).bind(0, true);

    while (Application::running) {
        Application::pollEvents();

        float speed = 15.0f * deltaTime;
        float cosVel = cos(cameraRot[1]) * speed;
        float sinVel = sin(cameraRot[1]) * speed;

        if (keyStates[KEY_W]){
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
        eng::Mat4f projMat = eng::Mat4f::GL_Projection(90.f, wWidth, wHeight, 0.1f, 100.f);
        eng::Mat4f viewMat = eng::Mat4f::xRotation(cameraRot[0])
            * eng::Mat4f::yRotation(cameraRot[1])
            * eng::Mat4f::translation((-cameraPos).data);

        eng::Mat4f mvpMat = projMat * viewMat;

        glClearColor(0.35f, 0.4f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO(CLASSIC));

        SHD(CLASSIC).use();

        glUniformMatrix4fv(mvpMatrix, 1, false, mvpMat.data);
        glUniformMatrix4fv(modMatrix, 1, false, modeMat.data);
        glUniform3fv(camPos, 1, cameraPos.data);
        glUniform3f(objColor, 0.8f, 0.7f, 0.45f);

        glDrawElements(GL_TRIANGLES, nCillinderIC(POINT_COUNT), GL_UNSIGNED_INT, (void*)(0));


        glDisable(GL_DEPTH_TEST);

        glBindVertexArray(VAO(RECTANGLE));
        SHD(RECTANGLE).use();

        glUniform2fv(rPosition, 1, rPos.data);
        glUniform2fv(rScale, 1, rScl.data);

        glDrawArrays(GL_TRIANGLES, 0, 6);



        SHD(RECT_COL).use();

        glUniform2fv(colPosition, 1, consolePos.data);
        glUniform2fv(colScale, 1, consoleScl.data);
        glUniform4f(colColor, 0.2f, 0.2f, 0.2f, 0.4f);

        glDrawArrays(GL_TRIANGLES, 0, 6);


        glEnable(GL_DEPTH_TEST);


        Text::consoleRender();


        SDL_GL_SwapWindow(Application::window);

        auto tp2 = std::chrono::high_resolution_clock::now();
        deltaTime = (float)((tp2 - tp1).count()) / 1000000000.f;
        timePassed += deltaTime;
        tp1 = tp2;
    }

    for (GraphicShader &shader : shaders) {
        shader.free();
    }

    for (Texture &texture : textures) {
        texture.free();
    }

    glDeleteBuffers(1, &BUF(CLASSIC_VBO));
    glDeleteBuffers(1, &BUF(CLASSIC_IBO));
    glDeleteVertexArrays(1, &VAO(CLASSIC));

    glDeleteBuffers(1, &BUF(RECTANGLE_VBO));
    glDeleteVertexArrays(1, &VAO(RECTANGLE));

    Text::exit();

    Application::close();

    return 0;
}

