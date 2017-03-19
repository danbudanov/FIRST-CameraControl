# FIRST Camera Control

This repository contains code used by the [Georgia FIRST Robotics Competition](http://gafirst.org/) for remote-controlling [PTZ Optics 12X-SDI](http://ptzoptics.com/12x-sdi/) cameras using [Logitech Extreme 3D Pro](http://gaming.logitech.com/en-us/product/extreme-3d-pro-joystick) joysticks.
 
 ## Setup
 
 #### Dependencies
 
 This application depends on [Cmake](), [Boost](), and [SDL 2.0](). In Linux, these can be installed with the following command:
 
 ```
 sudo apt-get install cmake libsdl2-2.0-0 libboost-all-dev
 ```
 
 #### Installation
 
 Clone this repository and run these commands from the FIRST-CameraControl folder:
 
 ```
 mkdir cmake-build-debug
 cd cmake-build-debug
 cmake ..
 make
 ```
 
 This will generate the FIRST_CameraControl binary executable in the cmake-build-debug folder.
 
 ## Usage
 
 