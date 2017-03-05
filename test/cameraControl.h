#ifndef __CameraControlSystem
#define __CameraControlSystem

namespace Camera{

	#define CAMERA_MAX_PAN 0x18
	#define CAMERA_MAX_TILT 0x14
	#define CAMERA_MAX_ZOOM 0x07

	int CameraFD;

	//pan speed - left and right speed
	//tilt speed - up and down speed
	void rotate(std::string type,unsigned char pan_speed,unsigned char tilt_speed){
		if(pan_speed>CAMERA_MAX_PAN)pan_speed=CAMERA_MAX_PAN;
		if(tilt_speed>CAMERA_MAX_TILT)tilt_speed=CAMERA_MAX_TILT;
		if(pan_speed==0x00)pan_speed=0x01;
		if(tilt_speed==0x00)tilt_speed=0x01;
		unsigned char mess[9]={0x81,0x01,0x06,0x01,pan_speed,tilt_speed,0x00,0x00,0xFF};

		if(type=="up"){
			mess[6]=0x03;
			mess[7]=0x01;
		}else if(type=="down"){
			mess[6]=0x03;
			mess[7]=0x02;
		}else if(type=="left"){
			mess[6]=0x01;
			mess[7]=0x03;
		}else if(type=="right"){
			mess[6]=0x02;
			mess[7]=0x03;
		}else if(type=="upLeft"){
			mess[6]=0x01;
			mess[7]=0x01;
		}else if(type=="upRight"){
			mess[6]=0x02;
			mess[7]=0x01;
		}else if(type=="downLeft"){
			mess[6]=0x01;
			mess[7]=0x02;
		}else if(type=="downRight"){
			mess[6]=0x02;
			mess[7]=0x02;
		}else if(type=="stop"){
			mess[6]=0x03;
			mess[7]=0x03;
		}else{return;} // not a proper command

		write(CameraFD,mess,9);
	}

	void image(std::string type,unsigned char power){
		if(power>0x07)power=CAMERA_MAX_ZOOM;
		unsigned char mess[6]={0x81,0x01,0x04,0x00,0x00,0xFF};

		if(type=="zoomStop"){
			mess[3]=0x07;
			mess[4]=0x00;
		}else if(type=="zoomTele"){
			mess[3]=0x07;
			mess[4]=0x02;
		}else if(type=="zoomWide"){
			mess[3]=0x07;
			mess[4]=0x03;
		}else if(type=="zoomTelePower"){
			mess[3]=0x07;
			mess[4]=0x20 | power; // gives us 0x2P
		}else if(type=="zoomWidePower"){
			mess[3]=0x07;
			mess[4]=0x30 | power; // gives us 0x3P
		//=========================================================
		}else if(type=="focusStop"){
			mess[3]=0x08;
			mess[4]=0x00;
		}else if(type=="focusFar"){
			mess[3]=0x08;
			mess[4]=0x02;
		}else if(type=="focusNear"){
			mess[3]=0x08;
			mess[4]=0x03;
		}else if(type=="focusFarPower"){
			mess[3]=0x08;
			mess[4]=0x20 | power; // gives us 0x2P
		}else if(type=="focusNearPower"){
			mess[3]=0x08;
			mess[4]=0x30 | power; // gives us 0x3P
		}else if(type=="focusAuto"){
			mess[3]=0x38;
			mess[4]=0x02;
		}else if(type=="focusManual"){
			mess[3]=0x38;
			mess[4]=0x03;
		}else if(type=="focusToggle"){
			mess[3]=0x38;
			mess[4]=0x10;
		}else{return;} // not a proper command

		write(CameraFD,mess,6);
	}

	void preset(std::string type, unsigned char value){
		unsigned char mess[7]={0x81,0x01,0x04,0x3F,0x00,0x00,0xFF};
		if(type=="storePreset"){
			mess[4]=0x01; // tell it to remember the current posistion
			mess[5]=value;
			write(CameraFD,mess,7);
		}else if(type=="recallPreset"){
			mess[4]=0x02; // tell it to remember the current posistion
			mess[5]=value;
			write(CameraFD,mess,7);
		}
	}

}
#endif