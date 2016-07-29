#include <Adafruit_TLC5947.h>

#define DEBUG

#define transDelay 0
#define stepSize 200

#define clk 4
#define dout 5
#define lat 6
#define oe -1


Adafruit_TLC5947 CCD(1, clk, dout, lat);

struct led{
	uint16_t ledNum;
	uint16_t rgb[3];
	uint16_t rgbGoal[3];
};
struct led* myLEDs;


void setup(){
	CCD.begin();

	#ifdef DEBUG
		Serial.begin(9600);
		Serial.println("Begin program");
	#endif

	if(oe >= 0){
		pinMode(oe, OUTPUT);
		digitalWrite(oe, LOW);
	}

	myLEDs = new led[8];
	int rgbIndex = 0;
	for(int i = 0; i < 4; i++){
		myLEDs[i].ledNum = i;
		myLEDs[i + 4].ledNum = i + 4;
		for(int j = 0; j < 3; j++){
			myLEDs[i].rgb[j] = 0;
			myLEDs[i].rgbGoal[j] = 0;
			myLEDs[i + 4].rgb[j] = 0;
			myLEDs[i + 4].rgbGoal[j] = 0;
		}

		myLEDs[i].rgb[rgbIndex] = 4095;
		myLEDs[i + 4].rgb[rgbIndex] = 4095;
		if(rgbIndex == 2){
			rgbIndex = 0;
		}
		else{
			rgbIndex++;
		}

		CCD.setLED(i, myLEDs[i].rgb[0], myLEDs[i].rgb[1], myLEDs[i].rgb[2]);
		CCD.setLED(i + 4, myLEDs[i + 4].rgb[0], myLEDs[i + 4].rgb[1], myLEDs[i + 4].rgb[2]);

		#ifdef DEBUG
			Serial.print("LED ");
			Serial.print(i);
			Serial.print(" & ");
			Serial.print(i + 4);
			Serial.println(" initialized\n");
		#endif
	}

	#ifdef DEBUG
		Serial.println("Post-initialization values:");
		for(int i = 0; i < 8; i++){
			Serial.print("RED");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgb[0]);
			
			Serial.print("GREEN");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgb[1]);
		
			Serial.print("BLUE");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgb[2]);

			Serial.println("");
		}
	#endif

	CCD.write();
}


void loop(){

}
