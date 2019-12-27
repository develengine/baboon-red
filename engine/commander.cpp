#include "commander.hpp"

#include <cstring>

#include "engutils.hpp"

namespace Commander {

const int ID_BUFFER_SIZE = 32;

struct Command
{
    char identifier[ID_BUFFER_SIZE];
    std::function<void(int, char**)> callback;
};

std::vector<Command> commands;

void nullReturn(int argc, char *argv[]) { }

void addCommand(const char *id, std::function<void(int, char**)> cb)
{
    Command newCommand;
    newCommand.callback = cb;
    eng::strcpymx(newCommand.identifier, id, ID_BUFFER_SIZE);

    commands.push_back(newCommand);
}

std::function<void(int, char**)> run(char *id)
{
    for (Command &command : commands)
    {
        if (strcmp(id, command.identifier) == 0)
        {
            return command.callback;
        }
    }

    return &nullReturn;
}

}

