#ifndef FIRST_CAMERACONTROL_CONSOLE_H
#define FIRST_CAMERACONTROL_CONSOLE_H

#include <thread>
#include <queue>
#include <map>

class Console {
public:
    Console();

    ~Console();

    std::queue<std::string> &command_queue(const std::string &cameraName);

private:
    void run();

    bool _running = true;

    std::thread _thread;

    std::map<std::string, std::queue<std::string>> _command_queues;

};


#endif //FIRST_CAMERACONTROL_CONSOLE_H
