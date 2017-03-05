#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string>

#ifndef __JoystickSystem
#define __JoystickSystem

/*
	A linux joystick thing. 
	This will only work on linux and 
*/
/*
Notes of what axies are what
0 - left right
1 - forward backward
2 - twist left right
3 - throttle
4 - hat left right
5 - hat forward backward
*/

class Joystick{
private:
	FILE* joystickFD;
	std::string devicePath;
	uint16_t deadzone; //how big is the dead zone - this is supposed to fit your specific application
	bool removeDeadzone; //should this class handle the deadzone or just pass back the raw values

	//a place to store all of our data in a easily readable manner
	uint8_t  crc;
	uint32_t packetNum;
	int16_t  data;
	uint8_t  dataType; // 1-buttons  2-axies
	uint8_t  inputNumber; // button 1 or axis 3 or something

	// keep track of the last known state of each of the different inputs
	int16_t axisData[6];
	int16_t buttonData[16];
    int16_t joystickPos;
    int prevZoom;
public:
	Joystick(){
		this->joystickFD=NULL; //default
		this->devicePath="/dev/input/js0";
		this->crc=0;
		this->packetNum=0;
		this->deadzone=0xFFFF/0x15;
		this->removeDeadzone=true;
		memset(axisData,0,6*2);
		memset(buttonData,0,16*2);
	}
	~Joystick(){
		if(this->joystickFD!=NULL) fclose(this->joystickFD);
	}
	//=====================================================================================
	// CONFIGURATION & INFORMATION
	// how this behaves and basics of what this class may return
	//=====================================================================================
	void setDeadzone(uint16_t value){
		this->deadzone=value;
	}
	uint16_t getDeadzone(){ return this->deadzone;}
	void enableDeadzone(){this->removeDeadzone=true;}
	void disableDeadzone(){this->removeDeadzone=false;}
	uint16_t getMaxAxisSize(){return (0xFFFF/2);}
	//=====================================================================================
	// RUNTIME
	// make the object do things as we are running
	// you can change joysticks as we go as well
	//=====================================================================================

    // next and prev zooms to more effectively use joystick zoom
    int getPrevZoom() {return prevZoom;}
    void setPrevZoom() {
        prevZoom = this->getZoomData();
    }

	bool connect(std::string devicePath){
		this->devicePath=devicePath;
		if(this->joystickFD!=NULL)fclose(this->joystickFD);
		this->joystickFD=fopen(devicePath.c_str(),"rb");
		return (this->joystickFD!=NULL); //true if we could connect to the joystick
	}
	void connect_tryUntillSucess(std::string devicePath){
		this->devicePath=devicePath;
		if(this->joystickFD!=NULL)fclose(this->joystickFD);
		this->joystickFD=fopen(devicePath.c_str(),"rb");
		while(this->joystickFD==NULL){
			printf("unable to get joystick (/dev/input/js0)\n");
			sleep(1);
			this->joystickFD=fopen(devicePath.c_str(),"rb");
		}
	}
	void update(){
		if(this->joystickFD==NULL)return;
		//BLOCKING call to get the next data from the joystick
		this->crc=fgetc(this->joystickFD);

		this->packetNum =fgetc(this->joystickFD);  //lower byte
		this->packetNum|=((unsigned int)fgetc(this->joystickFD))<<8; //middle byte
		this->packetNum|=((unsigned long)fgetc(this->joystickFD))<<16; //upper byte

		this->data =fgetc(this->joystickFD);
		this->data|=((int)fgetc(this->joystickFD))<<8;

		this->dataType=fgetc(this->joystickFD);
		this->inputNumber=fgetc(this->joystickFD);

		if(this->dataType==2){
			if(this->inputNumber<6) //protect from overflows
				this->axisData[this->inputNumber]=this->data;
		}else if(this->dataType==1){
			if(this->inputNumber<16) //protect from overflows
				this->buttonData[this->inputNumber]=this->data;
		}
	}

	//=====================================================================================
	// GET DATA
	// These get the user data about the joysticks current state
	// this is entierly just axis and button data
	//=====================================================================================

	//data from the previous update. this is always raw data
	uint8_t  getDataType(){return this->dataType;}
	uint8_t  getInputNumber(){return this->inputNumber;}
	int16_t  getData(){return this->data;}
	uint32_t getPacketNumber(){return this->packetNum;}
	uint8_t  getCRCCode(){return this->crc;}

	//the main things the user probably wants
	//get the parsed data from our memory
	int16_t getAxisData(int num){
		if(!this->removeDeadzone) return this->axisData[num];

		int axis = this->axisData[num];
		if(axis>-deadzone && axis<deadzone)return 0; //in the dead zone
		if(axis>0) return axis-deadzone;
		if(axis<0) return axis+deadzone;
		return 0;
	}

	int16_t getZoomData(){
		return this->axisData[3];
	}

	uint16_t getButtonData(int num){
		return this->buttonData[num];
	}
};

#endif
