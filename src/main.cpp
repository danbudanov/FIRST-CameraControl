#include <signal.h>
#include <iostream>
#include "Joystick.h"
#include "Camera.h"
#include "Controller.h"

using namespace std;

volatile bool running = true;

void signal_handler(int) {
    running = false;
}

void set_SIGINT_handler() {
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = signal_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
}

int main(int argc, char* argv[]){

    Joystick::Init();

    std::vector<std::string> camera_IPs = {
            "10.44.44.51",
            "10.44.44.52",
            "10.44.44.53"
    };

    boost::asio::io_service io_service;

    auto joystick_count = Joystick::NumberOfConnectJoysticks();

    cout << "Found " << joystick_count << " joysticks." << endl;

    cout << "Conecting to " << min(joystick_count, static_cast<int>(camera_IPs.size())) << " cameras." << endl;

    std::vector<Controller> controllers;

    for(auto i = 0; i < joystick_count && i < camera_IPs.size(); ++i) {
        cout << "Assigning joystick " << i << " to camera at " << camera_IPs[i] << "." << endl;
        controllers.emplace_back(make_shared<Joystick>(i), make_shared<Camera>(camera_IPs[i], io_service));
    }

    for(auto &controller : controllers) {
        controller.joystick()->set_all_deadzones(1.0/20.0);
    }

	while(running){
        for(auto &controller : controllers) {
            controller.update();
        }
	}

    return 0;
}
