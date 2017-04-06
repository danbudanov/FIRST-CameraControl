#include <iostream>
#include "Console.h"

using namespace std;

Console::Console()
    : _thread(&Console::run, this)
{

}

Console::~Console() {
    _running = false;
    _thread.join();
}

std::queue<std::string> &Console::command_queue(const std::string &cameraName) {
    return _command_queues[cameraName];
}

void Console::run() {
    while(_running) {
        string command;

        getline(cin, command);

        auto cameraName = command.substr(0, command.find(' '));

        command = command.substr(command.find(' '));

        _command_queues[cameraName].push(command);
    }
}
