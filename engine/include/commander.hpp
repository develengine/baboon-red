#pragma once

#include <functional>

namespace Commander {

extern const int ID_BUFFER_SIZE;

void addCommand(const char *id, std::function<void(int, char**)> cb);

std::function<void(int, char**)> run(char *id);

}

