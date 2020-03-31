#include "console.hpp"

#include <glad/glad.h>

#include "engutils.hpp"
#include "oglcore.hpp"
#include "commander.hpp"
#include "application.hpp"

namespace Console {

const int LINE_LENGTH = 128;
const int LINE_COUNT  = 64;
const int VISIBLE_LINE_COUNT = 32;
const int DEVICE_MEMORY_OFFSET = 0;
const int INPUT_LINE_SIZE = CONSOLE_INPUT_LINE_SIZE;
// TODO @Later: Add debug exception messages
const int MAX_ARG_COUNT = 32;

u8 lineInProgress[INPUT_LINE_SIZE] = { 0 };
bool inputActive = false;

// const int DEVICE_MEMORY_SIZE = LINE_LENGTH * LINE_COUNT;

bool active = false;

eng::Vec2f consolePosition(-1.0f, 1.0f);

int lineOffset = 0;

struct ConsoleLine {
    u8 characters[LINE_LENGTH];
};

eng::WrapQueue<ConsoleLine, LINE_COUNT> lineBuffer;
eng::WrapQueue<int, LINE_COUNT> lineLengths;

int newLineCount = 0;


void write(const u8 *text)
{
    ConsoleLine *line;

    u8 c;
    int i = 0;
    int lineStart = 0;
    int lineSize = 0;
    while (true)
    {
        if ((c = text[i]) == 0 || lineSize >= LINE_LENGTH)
        {
            line = &(lineBuffer.get());
            memcpy(line->characters, &(text[lineStart]), lineSize);
            lineLengths.push(lineSize);
            ++newLineCount;

            lineSize = 0;
            lineStart = i;

            if (c == 0) break;
        }

        ++i;
        ++lineSize;
    }
}

void write(const char *text)
{
    ConsoleLine *line;

    u8 c;
    int i = 0;
    int lineStart = 0;
    int lineSize = 0;
    while (true)
    {
        if ((c = text[i]) == 0 || lineSize >= LINE_LENGTH)
        {
            line = &(lineBuffer.get());
            eng::strToUStrLen(&(text[lineStart]), line->characters, lineSize);
            lineLengths.push(lineSize);
            ++newLineCount;

            lineSize = 0;
            lineStart = i;

            if (c == 0) break;
        }

        ++i;
        ++lineSize;
    }
}

void inputCallback(TextEdit::ReturnState s)
{
    switch (s)
    {
        case TextEdit::ENTRY:
        {
            char inputCopy[INPUT_LINE_SIZE];
            int inputLength = eng::ustrlen(lineInProgress);
            memcpy(inputCopy, lineInProgress, inputLength + 1);

            char *arguments[MAX_ARG_COUNT];
            int argumentCount = 0;

            int i = 0;
            char c;
            int argStart = 0;
            while ((c = inputCopy[i]) != 0)
            {
                if (c == ' ')
                {
                    arguments[argumentCount++] = &(inputCopy[argStart]);
                    argStart = i + 1;
                    inputCopy[i] = 0;

                    if (argumentCount >= MAX_ARG_COUNT - 1) break;
                }

                ++i;
            }

            if (argumentCount < MAX_ARG_COUNT - 1)
            {
                arguments[argumentCount++] = &(inputCopy[argStart]);
            }

            write(lineInProgress);

            Commander::run(arguments[0])(argumentCount - 1, &(arguments[1]));

            lineInProgress[0] = 0;
            TextEdit::setTarget(
                lineInProgress,
                INPUT_LINE_SIZE,
                &inputCallback
            );

            break;
        }
        case TextEdit::TERMINATION:

            inputActive = false;

            break;
    }
}

void render()
{
    // TODO @Performance: Merge glBufferSubData calls into one/two

    glBindBuffer(GL_ARRAY_BUFFER, ENG_BUF(TEXT_STRING));
    int cycles = std::min(LINE_COUNT, newLineCount);
    int queueSize = lineLengths.size();

    for (int i = queueSize - 1; i > queueSize - 1 - cycles; i--)
    {
        int localOffset = LINE_LENGTH * lineLengths.position(i);
        glBufferSubData( GL_ARRAY_BUFFER,
            DEVICE_MEMORY_OFFSET + localOffset, 
            lineLengths[i],
            lineBuffer[i].characters
        );
    }

    int inputLineLength = eng::ustrlen(lineInProgress);

    glBufferSubData( GL_ARRAY_BUFFER,
        DEVICE_MEMORY_OFFSET + LINE_LENGTH * LINE_COUNT,
        inputLineLength + 1,
        lineInProgress
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    newLineCount = 0;


    int wWidth, wHeight;
    SDL_GetWindowSize(Application::window, &wWidth, &wHeight);

    // Background rendering
    glBindVertexArray(ENG_VAO(RECTANGLE));
    glUseProgram(ENG_PRG(RECT_COL));

    eng::Vec2f characterSize(8.f / (float)(wWidth / 2), 16.f / (float)(wHeight / 2));

    eng::Vec2f consoleScl(
        characterSize[0] * LINE_LENGTH,
        characterSize[1] * (VISIBLE_LINE_COUNT + 2)
    );
    glUniform2fv(ENG_UNI(RECT_COL_POSITION), 1, consolePosition.data);
    glUniform2fv(ENG_UNI(RECT_COL_SCALE), 1, consoleScl.data);
    glUniform4f(ENG_UNI(RECT_COL_COLOR), 0.05f, 0.05f, 0.05f, 0.6f);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Text rendering
    glBindVertexArray(ENG_VAO(TEXT));
    glUseProgram(ENG_PRG(TEXT));
    
    glUniform1i(ENG_UNI(TEXT_WRAP), LINE_LENGTH);
    glUniform4f(ENG_UNI(TEXT_COLOR), 1.0f, 1.0f, 1.0f, 1.0f);
    glUniform4f(ENG_UNI(TEXT_BACKGROUND), 0.0f, 0.0f, 0.0f, 0.0f);

    // TODO [console]: color; line folding; scrolling

    int lineCount = std::min(VISIBLE_LINE_COUNT, lineLengths.size() - lineOffset);

    int linePosition = 0;
    for (int i = queueSize - lineCount + lineOffset; i < queueSize; i++)
    {
        glUniform4f( ENG_UNI(TEXT_TRANSFORM),
            consolePosition[0], consolePosition[1] - characterSize[1] * linePosition,
            characterSize[0], characterSize[1]
        );
        glDrawArraysInstancedBaseInstance( GL_TRIANGLES,
            0, 6,
            lineLengths[i + lineOffset],
            DEVICE_MEMORY_OFFSET + lineLengths.position(i + lineOffset) * LINE_LENGTH
        );

        ++linePosition;
    }

    // Typing line rendering

    glUniform4f( ENG_UNI(TEXT_TRANSFORM),
        consolePosition[0],
        consolePosition[1] - characterSize[1] * VISIBLE_LINE_COUNT - characterSize[1] / 2,
        characterSize[0], characterSize[1]
    );

    if (inputActive)
    {
        int cursorPosition = TextEdit::position;
        int lineSize = eng::ustrlen(lineInProgress) + 1;

        glUniform4f(ENG_UNI(TEXT_COLOR), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform4f(ENG_UNI(TEXT_BACKGROUND), 0.0f, 0.0f, 0.0f, 0.0f);

        glDrawArraysInstancedBaseInstance( GL_TRIANGLES,
            0, 6,
            cursorPosition,
            DEVICE_MEMORY_OFFSET + LINE_COUNT * LINE_LENGTH
        );

        glUniform4f( ENG_UNI(TEXT_TRANSFORM),
            consolePosition[0] + characterSize[0] * (cursorPosition + 1),
            consolePosition[1] - characterSize[1] * VISIBLE_LINE_COUNT - characterSize[1] / 2,
            characterSize[0], characterSize[1]
        );

        glDrawArraysInstancedBaseInstance( GL_TRIANGLES,
            0, 6,
            lineSize - cursorPosition - 1,
            DEVICE_MEMORY_OFFSET + LINE_COUNT * LINE_LENGTH + cursorPosition + 1
        );

        glUniform4f( ENG_UNI(TEXT_TRANSFORM),
            consolePosition[0] + characterSize[0] * cursorPosition,
            consolePosition[1] - characterSize[1] * VISIBLE_LINE_COUNT - characterSize[1] / 2,
            characterSize[0], characterSize[1]
        );
        glUniform4f(ENG_UNI(TEXT_BACKGROUND), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform4f(ENG_UNI(TEXT_COLOR), 0.0f, 0.0f, 0.0f, 0.0f);

        glDrawArraysInstancedBaseInstance( GL_TRIANGLES,
            0, 6,
            1,
            DEVICE_MEMORY_OFFSET + LINE_COUNT * LINE_LENGTH + cursorPosition
        );
    }
    else
    {
        glUniform4f(ENG_UNI(TEXT_COLOR), 1.0f, 1.0f, 1.0f, 1.0f);
        glUniform4f(ENG_UNI(TEXT_BACKGROUND), 0.0f, 0.0f, 0.0f, 0.0f);

        glDrawArraysInstancedBaseInstance( GL_TRIANGLES,
            0, 6,
            inputLineLength,
            DEVICE_MEMORY_OFFSET + LINE_COUNT * LINE_LENGTH
        );
    }

}

}

