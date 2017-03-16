#include <iostream>
#include "Controller.h"

using namespace std;

Controller::Controller(std::shared_ptr<Joystick> joystick, std::shared_ptr<Camera> camera)
    : _joystick(joystick),
      _camera(camera)
{
}

void Controller::update() {
    if(_joystick != nullptr && _camera != nullptr) {
        _joystick->update();

        axis_control();

        button_control();
    }
}

void Controller::change_joystick(std::shared_ptr<Joystick> joystick) {
    _joystick = joystick;
}

void Controller::chage_camera(std::shared_ptr<Camera> camera) {
    _camera = camera;
}

void Controller::axis_control() {
    _camera->rotate(_joystick->axis(0), _joystick->axis(1));

    // TODO wut is this differential zoom

    auto zoom = _joystick->axis(3);

    cout << zoom << endl;

    auto delta_zoom = zoom - _prev_zoom;

    _camera->zoom(delta_zoom);

    _prev_zoom = zoom;
}

void Controller::button_control() {
    // Emergency stop button
    if(_joystick->button(0)){
        _camera->stop();
        printf("\tEmergency Stop\n");
    }

    // Change camera
    if(_joystick->button(2)){
        _camera->stop();
        // Camera::CameraFD=CameraFD1; //change the place we are sending the commands
        printf("\tChanging to Camera1\n");
    }else if(_joystick->button(3)){
        _camera->stop();
        // Camera::CameraFD=CameraFD2; //change the place we are sending the commands
        printf("\tChanging to Camera2\n");
    }else if(_joystick->button(4)){
        _camera->stop();
        // Camera::CameraFD=CameraFD3; //change the place we are sending the commands
        printf("\tChanging to Camera3\n");
    }

    // Save a preset
    if(_joystick->button(5)){
        _camera->stop();

        if(_joystick->button(6)){
            _camera->save_preset(0);
            cout << "\tSetting preset 0\n";
        }else if(_joystick->button(7)){
            _camera->save_preset(1);
            cout << "\tSetting preset 1\n";
        }else if(_joystick->button(8)){
            _camera->save_preset(2);
            cout << "\tSetting preset 2\n";
        }else if(_joystick->button(9)){
            _camera->save_preset(3);
            cout << "\tSetting preset 3\n";
        }else if(_joystick->button(10)){
            _camera->save_preset(4);
            cout << "\tSetting preset 4\n";
        }else if(_joystick->button(11)){
            _camera->save_preset(5);
            cout << "\tSetting preset 5\n";
        }
    }

    // Go to a preset
    else{
        if(_joystick->button(6)){
            _camera->recall_preset(0);
            cout << "\tRecalling preset 0\n";
        }else if(_joystick->button(7)){
            _camera->recall_preset(1);
            cout << "\tRecalling preset 1\n";
        }else if(_joystick->button(8)){
            _camera->recall_preset(2);
            cout << "\tRecalling preset 2\n";
        }else if(_joystick->button(9)){
            _camera->recall_preset(3);
            cout << "\tRecalling preset 3\n";
        }else if(_joystick->button(10)){
            _camera->recall_preset(4);
            cout << "\tRecalling preset 4\n";
        }else if(_joystick->button(11)){
            _camera->recall_preset(5);
            cout << "\tRecalling preset 5\n";
        }
    }
}
