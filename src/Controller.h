#ifndef FIRST_CAMERACONTROL_CONTROLLER_H
#define FIRST_CAMERACONTROL_CONTROLLER_H

#include "Joystick.h"
#include "Camera.h"
#include "Console.h"

class Controller {
public:
    Controller(std::shared_ptr<Joystick> joystick = nullptr, std::shared_ptr<Camera> camera = nullptr, std::shared_ptr<Console> console = nullptr);

    void update();

    void change_joystick(std::shared_ptr<Joystick> joystick);

    void chage_camera(std::shared_ptr<Camera> camera);

    std::shared_ptr<Joystick> joystick() const;

    std::shared_ptr<Camera> camera() const;

private:
    std::shared_ptr<Joystick> _joystick;

    std::shared_ptr<Camera> _camera;

    std::shared_ptr<Console> _console;

    Joystick::HatDirection _prev_hat = Joystick::HatDirection::CENTERED;
    double _prev_pan = 0;
    double _prev_tilt = 0;
    double _prev_speed = 0;

    std::array<bool,12> _prev_button;

    void axis_control();

    void button_control();

    void set_prev_buttons();

    void console_control();

};

#endif //FIRST_CAMERACONTROL_CONTROLLER_H
