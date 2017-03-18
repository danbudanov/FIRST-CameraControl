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

int main(){

    Joystick::Init();

    /*
     * TODO make a config file that contains camera IPs, names, and Joystick GUIDs and their mappings.
     */

    vector<pair<string, string>> camera_details {
            {"Blue", "10.44.44.51"},
            {"Center", "10.44.44.52"},
            {"Red", "10.44.44.53"}
    };

    boost::asio::io_service io_service;

    size_t joystick_count = static_cast<size_t>(Joystick::NumberOfConnectJoysticks());

    cout << "Found " << joystick_count << " joysticks." << endl;

    cout << "Conecting to " << min(joystick_count, static_cast<size_t>(camera_details.size())) << " cameras." << endl;

    std::vector<Controller> controllers;

    for(size_t i = 0; i < joystick_count && i < camera_details.size(); ++i) {
        cout << "Assigning joystick " << i << " to camera " << camera_details[i].first << "(" << camera_details[i].second << ")." << endl;
        controllers.emplace_back(make_shared<Joystick>(i), make_shared<Camera>(camera_details[i].first, camera_details[i].second, io_service));
    }

    for(auto &controller : controllers) {
        controller.joystick()->set_deadzone(0, 0.25);
        controller.joystick()->set_deadzone(1, 0.25);
    }

	while(running){
        for(auto &controller : controllers) {
            controller.update();
        }
	}

    return 0;
}
