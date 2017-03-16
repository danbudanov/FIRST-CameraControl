#ifndef FIRST_CAMERACONTROL_JOYSTICK2_H
#define FIRST_CAMERACONTROL_JOYSTICK2_H

#include <SDL2/SDL_joystick.h>
#include <memory>
#include <vector>

struct SDLJoystickDeleter {
    inline void operator()(SDL_Joystick *joy) {
        if(SDL_JoystickGetAttached(joy)) {
            SDL_JoystickClose(joy);
        }
    }
};

class Joystick {
public:
    Joystick(int index);

    bool is_open() const;

    void update();

    double axis(int index);

    bool button(int index);

    void set_deadzone(int index, double value);

    void set_all_deadzones(double value);

    static void Init();

    static int NumberOfConnectJoysticks();

private:
    std::unique_ptr<SDL_Joystick, SDLJoystickDeleter > _joystick;

    std::vector<double> _deadzones;

    SDLJoystickDeleter _joy_deleter;

    static constexpr int16_t axis_max = 32767;
    static constexpr int16_t axis_min = -32767;
};


#endif //FIRST_CAMERACONTROL_JOYSTICK2_H
