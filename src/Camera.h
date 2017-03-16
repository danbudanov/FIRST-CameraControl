#ifndef FIRST_CAMERACONTROL_CAMERA_H
#define FIRST_CAMERACONTROL_CAMERA_H

#include <cstdint>
#include <vector>
#include <boost/asio.hpp>

class Camera {
public:
    Camera(std::string ip_address, boost::asio::io_service &io_service);

    ~Camera();

    void save_preset(uint8_t number);

    void recall_preset(uint8_t number);

    void rotate(double pan = 0, double tilt = 0);

    enum class ZoomType {
        TELE,
        WIDE
    };

    void zoom(double speed = 0);

    void zoom(ZoomType type);

    enum class FocusType {
        STOP,
        FAR,
        NEAR,
        AUTO,
        MANUAL,
        TOGGLE
    };

    void focus(double speed = 0);

    void focus(FocusType type);

    void stop();

    static inline uint8_t pan_max() {
        return 24;
    }

    static inline uint8_t tilt_max() {
        return 20;
    }

    static inline uint8_t zoom_max() {
        return 7;
    }

private:

    boost::asio::io_service& _io_service;
    boost::asio::ip::udp::socket _socket;
    boost::asio::ip::udp::endpoint _endpoint;

    void send_command(std::vector<uint8_t> command);

};


#endif //FIRST_CAMERACONTROL_CAMERA_H
