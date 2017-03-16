#include <cmath>
#include <iostream>
#include "Camera.h"

using namespace std;
using boost::asio::ip::udp;

Camera::Camera(std::string ip_address, boost::asio::io_service &io_service)
    : _io_service(io_service),
      _socket(_io_service, udp::endpoint(udp::v4(), 0))
{
    udp::resolver resolver(_io_service);
    udp::resolver::query query(udp::v4(), ip_address, "5678");
    udp::resolver::iterator resolver_iterator = resolver.resolve(query);
    _endpoint = *resolver_iterator;

    /*
     * TODO open camera
     * Boost::ASIO UDP example: https://gist.github.com/kaimallea/e112f5c22fe8ca6dc627
     *
     * addresses: 10.44.44.[51|52|53]
     * port numer: 5678
     *
     */
}

Camera::~Camera() {
    _socket.close();
}

void Camera::save_preset(uint8_t number) {
    send_command({0x81, 0x01, 0x04, 0x3F, 0x01, number, 0xFF});
}

void Camera::recall_preset(uint8_t number) {
    send_command({0x81, 0x01, 0x04, 0x3F, 0x02, number, 0xFF});
}

void Camera::rotate(double pan, double tilt) {
    uint8_t pan_power = static_cast<uint8_t>(pan_max() * abs(pan));
    pan_power = max(pan_power, static_cast<uint8_t>(1));

    uint8_t tilt_power = static_cast<uint8_t>(tilt_max() * abs(tilt));
    tilt_power = max(tilt_power, static_cast<uint8_t>(1));

    uint8_t pan_direction = 0x03;
    if(pan < 0)
        pan_direction = 0x02;
    else if(pan > 0)
        pan_direction = 0x01;

    uint8_t tilt_direction = 0x03;
    if(tilt < 0)
        tilt_direction = 0x02;
    else if(tilt > 0)
        tilt_direction = 0x01;

    send_command({0x81,0x01,0x06,0x01,pan_power,tilt_power,pan_direction,tilt_direction,0xFF});
}

void Camera::zoom(double speed) {
    uint8_t power = static_cast<uint8_t>(zoom_max() * abs(speed));

    uint8_t direction = 0x00;
    if(speed < 0)
        direction = static_cast<uint8_t>(0x30) | power;
    else if(speed > 0)
        direction = static_cast<uint8_t>(0x20) | power;

    send_command({0x81,0x01,0x04,0x07,direction,0xFF});
}

void Camera::zoom(Camera::ZoomType type) {
    uint8_t direction;
    switch(type) {
        case ZoomType::TELE:
            direction = 0x02;
            break;
        case ZoomType::WIDE:
            direction = 0x03;
            break;
    }
    send_command({0x81,0x01,0x04,0x07,direction,0xFF});
}

void Camera::focus(double speed) {
    // TODO Not used yet, so not implemented yet
}

void Camera::focus(Camera::FocusType type) {
    // TODO Not used yet, so not implemented yet
}

void Camera::stop() {
    rotate();
    zoom();
}

void Camera::send_command(std::vector<uint8_t> command) {
    _socket.send_to(boost::asio::buffer(command.data(), command.size()), _endpoint);
}
