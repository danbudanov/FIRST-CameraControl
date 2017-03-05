#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <cmath>
#include "../libs/stdSocketTools.h"
#include "joystick.h"
#include "cameraControl.h"

int CameraFD1=2;
int CameraFD2=2;
int CameraFD3=2;
//int CameraFD1=simpleConnectToHost("10.42.0.101",5678);
//int CameraFD2=simpleConnectToHost("10.42.0.102",5678);
//int CameraFD3=simpleConnectToHost("10.42.0.103",5678);

void axisControl(Joystick* joystick);
void buttonControl(Joystick* joystick);

int main(){
	//Camera::CameraFD=2; //lol, print commands to stderr - just for debug running so we can remove the noise
	CameraFD1=simpleConnectToHost("10.44.44.52",5678);
	while(CameraFD1<=0) CameraFD1=simpleConnectToHost("10.44.44.52",5678);
	CameraFD2=simpleConnectToHost("10.44.44.51",5678);
	while(CameraFD2<=0) CameraFD2=simpleConnectToHost("10.44.44.51",5678);
	CameraFD3=simpleConnectToHost("10.44.44.53",5678);
	while(CameraFD3<=0) CameraFD3=simpleConnectToHost("10.44.44.53",5678);

	Camera::CameraFD=CameraFD1; //start with the first camera
	Joystick joystick;
	joystick.connect_tryUntillSucess("/dev/input/js0"); //make our joystick available to us
	joystick.setDeadzone(joystick.getMaxAxisSize()/0x14);

	printf("CameraFD1 : %d\n",CameraFD1);
	printf("CameraFD2 : %d\n",CameraFD2);
	printf("CameraFD3 : %d\n",CameraFD3);

	int axisData[6]; // for each axis we will have a value
	int buttonData[16]; // for each button we will have a value
    int lastInput = joystick.getInputNumber();
    int enablePT = 0;



	while(1){
		joystick.update();
		//joystick axis has moved
		if(joystick.getDataType()==2 && lastInput==1 && enablePT){
			printf("Found axis %u with data %d\n",joystick.getInputNumber(),joystick.getData());
			axisControl(&joystick);
		}
		else if(joystick.getDataType()==1){
            //printf("Zoom is %d",  joystick.getZoomData());
            lastInput = joystick.getInputNumber();
            enablePT = (lastInput == 1) && joystick.getData();
            // keeps track of previous position on joysticfk
            if(enablePT) {
                joystick.setPrevZoom();
            } else {
                Camera::image("zoomStop", 1);
            }
            //

			printf("Found button %u and enablePT is %d\n",lastInput,enablePT);
			buttonControl(&joystick);
		}else {
            Camera::rotate("stop",1,1);
            Camera::image("zoomStop", 1);
        }
	}

	return 0;
}

void axisControl(Joystick* joystick){

	//we have gotten a new update for an axis and so we must change our camera stuffs

	/*
	How to scale the axies
	d = deadzone size
	r = range of steps that we want to scale to
	c = current value to scale
	u = usable range
	u = MAX - d #basicly the range outside the dead zone

	SCALED = r * (c-d)/u
	get the percentage of the current value over the usable range
	then multiply that percentage to the new range we want
	*/
    
    // zoom stuffs
    int ZP, ZPn;

	if(joystick->getInputNumber()==0 || joystick->getInputNumber()==1){
		//this is a simple left right up down
		//there are 9 cases - one of the axies are in the dead zone (4), neither are(4), both are(1)
		if(joystick->getAxisData(0)==0 && joystick->getAxisData(1)==0){
			Camera::rotate("stop",1,1);
			printf("LR :: UD  ==  %6d :: %6d             \n",0,0);
		}else if(joystick->getAxisData(0)>0 && joystick->getAxisData(1)==0){
			int UD=1; //must have a value to just call the function
			int LR=joystick->getAxisData(0);
			LR-=joystick->getDeadzone();
			LR=ceil(CAMERA_MAX_PAN * ( ((double)LR)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			LR+=2; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers
			Camera::rotate("right",LR / 2, UD / 2);
			printf("LR :: UD  ==  %6d :: %6d        right\n",LR,UD);
		}else if(joystick->getAxisData(0)<0 && joystick->getAxisData(1)==0){
			int UD=1; //must have a value to just call the function
			int LR=joystick->getAxisData(0) * -1;
			LR-=joystick->getDeadzone();
			LR=ceil(CAMERA_MAX_PAN * ( ((double)LR)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			LR+=2; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers
			Camera::rotate("left",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d        left \n",LR,UD);
		}else if(joystick->getAxisData(0)==0 && joystick->getAxisData(1)>0){
			int UD=joystick->getAxisData(1); //must have a value to just call the function
			UD-=joystick->getDeadzone();
			UD=ceil(CAMERA_MAX_TILT * ( ((double)UD)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			UD+=1; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers
			int LR=1;
			Camera::rotate("up",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d  up         \n",LR,UD);
		}else if(joystick->getAxisData(0)==0 && joystick->getAxisData(1)<0){
			int UD=joystick->getAxisData(1) * -1;
			UD-=joystick->getDeadzone();
			UD=ceil(CAMERA_MAX_TILT * ( ((double)UD)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			UD+=1; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers
			int LR=1;
			Camera::rotate("down",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d  down       \n",LR,UD);
		}else if(joystick->getAxisData(0)>0 && joystick->getAxisData(1)>0){
			int UD=joystick->getAxisData(1);
			UD-=joystick->getDeadzone();
			UD=ceil(CAMERA_MAX_TILT * ( ((double)UD)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			UD+=1; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			int LR=joystick->getAxisData(0);
			LR-=joystick->getDeadzone();
			LR=ceil(CAMERA_MAX_PAN * ( ((double)LR)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			LR+=2; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			Camera::rotate("upRight",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d  up    right\n",LR,UD);
		}else if(joystick->getAxisData(0)<0 && joystick->getAxisData(1)>0){
			int UD=joystick->getAxisData(1);
			UD-=joystick->getDeadzone();
			UD=ceil(CAMERA_MAX_TILT * ( ((double)UD)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			UD+=1; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			int LR=joystick->getAxisData(0) * -1;
			LR-=joystick->getDeadzone();
			LR=ceil(CAMERA_MAX_PAN * ( ((double)LR)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			LR+=2; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			Camera::rotate("upLeft",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d  up    left \n",LR,UD);
		}else if(joystick->getAxisData(0)>0 && joystick->getAxisData(1)<0){
			int UD=joystick->getAxisData(1) * -1;
			UD-=joystick->getDeadzone();
			UD=ceil(CAMERA_MAX_TILT * ( ((double)UD)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			UD+=1; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			int LR=joystick->getAxisData(0);
			LR-=joystick->getDeadzone();
			LR=ceil(CAMERA_MAX_PAN * ( ((double)LR)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			LR+=2; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			Camera::rotate("downRight",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d  down  right\n",LR,UD);
		}else if(joystick->getAxisData(0)<0 && joystick->getAxisData(1)<0){
			int UD=joystick->getAxisData(1) * -1;
			UD-=joystick->getDeadzone();
			UD=ceil(CAMERA_MAX_TILT * ( ((double)UD)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			UD+=1; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			int LR=joystick->getAxisData(0) * -1;
			LR-=joystick->getDeadzone();
			LR=ceil(CAMERA_MAX_PAN * ( ((double)LR)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
			LR+=2; //stupid rounding errors make it 2 too small - sometimes giving us neg numbers

			Camera::rotate("downLeft",LR,UD);
			printf("LR :: UD  ==  %6d :: %6d  down  left \n",LR,UD);
		}
	}else if(joystick->getInputNumber()==3){

			ZP = joystick->getData() ;//* -1;
            ZP = ZP - joystick->getPrevZoom();
			// ZP=ceil(CAMERA_MAX_ZOOM * ( ((double)ZP)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
            ZPn=ceil(CAMERA_MAX_ZOOM * ( ((double)ZP)/(joystick->getMaxAxisSize()) ));
		if(ZPn==0){
			Camera::image("zoomStop",1);
			printf("ZP ::     == %6d\n",0);
		}else if(ZPn > 0){
			//ZP = joystick->getData() ;//* -1;
            //ZP = ZP - joystick->getPrevZoom();
			//ZP-=joystick->getDeadzone();
			// ZP=ceil(CAMERA_MAX_ZOOM * ( ((double)ZP)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));
            //ZPn=ceil(CAMERA_MAX_ZOOM * ( ((double)ZP)/(joystick->getMaxAxisSize()) ));

            printf("Zoom wide: %d\n", ZP);

			Camera::image("zoomWidePower",ZPn);
			printf("ZP ::     == %6d         zoom       \n",ZP);
		}else if(ZPn < 0){
            ZPn = -1 * ZPn;
            printf("Zoom tele: %d\n", ZP);

			Camera::image("zoomTelePower",ZPn);
			printf("ZP ::     == %6d         tele       \n",ZP);
		}else {
			Camera::image("zoomStop",1);
			printf("ZP ::     == %6d\n",0);
		}

	}
    /*else if(joystick->getInputNumber()==2){
		if(joystick->getAxisData(2)==0){
			Camera::image("zoomStop",1);
			printf("ZP ::     == %6d\n",0);
		}else if(joystick->getAxisData(2)>0){
			int ZP = joystick->getAxisData(2);
			ZP-=joystick->getDeadzone();
			ZP=ceil(CAMERA_MAX_ZOOM * ( ((double)ZP)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));

			Camera::image("zoomTelePower",ZP);
			printf("ZP ::     == %6d         zoom       \n",ZP);
		}else if(joystick->getAxisData(2)<0){
			int ZP = joystick->getAxisData(2) * -1;
			ZP-=joystick->getDeadzone();
			ZP=ceil(CAMERA_MAX_ZOOM * ( ((double)ZP)/(joystick->getMaxAxisSize()-joystick->getDeadzone()) ));

			Camera::image("zoomWidePower",ZP);
			printf("ZP ::     == %6d         tele       \n",ZP);
		}else {
			Camera::image("zoomStop",1);
			printf("ZP ::     == %6d\n",0);
		}
	}*/
}

void buttonControl(Joystick* joystick){
	//emergency stop button - just in case
	if(joystick->getButtonData(0)==1){
		Camera::image("zoomStop",1);
		Camera::rotate("stop",1,1);
		printf("\tEmergency Stop\n");
	}
	//change camera buttons
	else if(joystick->getButtonData(2)==1){
		Camera::image("zoomStop",1);
		Camera::rotate("stop",1,1);
		Camera::CameraFD=CameraFD1; //change the place we are sending the commands
		printf("\tChanging to Camera1\n");
	}else if(joystick->getButtonData(3)==1){
		Camera::image("zoomStop",1);
		Camera::rotate("stop",1,1);
		Camera::CameraFD=CameraFD2; //change the place we are sending the commands
		printf("\tChanging to Camera2\n");
	}else if(joystick->getButtonData(4)==1){
		Camera::image("zoomStop",1);
		Camera::rotate("stop",1,1);
		Camera::CameraFD=CameraFD3; //change the place we are sending the commands
		printf("\tChanging to Camera3\n");
	}
	//set a preset
	if(joystick->getButtonData(5)==1){
		Camera::image("zoomStop",1);
		Camera::rotate("stop",1,1);
		printf("\tReady to set a preset\n");
		if(joystick->getButtonData(6)==1){
			Camera::preset("storePreset",0);
			printf("\tSetting preset 0\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(7)==1){
			Camera::preset("storePreset",1);
			printf("\tSetting preset 1\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(8)==1){
			Camera::preset("storePreset",2);
			printf("\tSetting preset 2\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(9)==1){
			Camera::preset("storePreset",3);
			printf("\tSetting preset 3\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(10)==1){
			Camera::preset("storePreset",4);
			printf("\tSetting preset 4\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(11)==1){
			Camera::preset("storePreset",5);
			printf("\tSetting preset 5\n"); //remember - we zero index but buttons are 1 indexed
		}
	}
	//visit a preset
	else{
		if(joystick->getButtonData(6)==1){
			Camera::preset("recallPreset",0);
			printf("\tRecalling preset 0\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(7)==1){
			Camera::preset("recallPreset",1);
			printf("\tRecalling preset 1\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(8)==1){
			Camera::preset("recallPreset",2);
			printf("\tRecalling preset 2\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(9)==1){
			Camera::preset("recallPreset",3);
			printf("\tRecalling preset 3\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(10)==1){
			Camera::preset("recallPreset",4);
			printf("\tRecalling preset 4\n"); //remember - we zero index but buttons are 1 indexed
		}else if(joystick->getButtonData(11)==1){
			Camera::preset("recallPreset",5);
			printf("\tRecalling preset 5\n"); //remember - we zero index but buttons are 1 indexed
		}
	}
}
