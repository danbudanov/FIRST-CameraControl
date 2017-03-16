#include <signal.h>
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

    boost::asio::io_service io_service;

    shared_ptr<Joystick> joystick = make_shared<Joystick>(0);

    shared_ptr<Camera> camera = make_shared<Camera>("10.44.44.51", io_service);

    joystick->set_all_deadzones(1.0/20.0);

    Controller controller{joystick, camera};

	while(running){
        controller.update();
	}

    return 0;
}
