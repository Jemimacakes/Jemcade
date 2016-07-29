#include <Adafruit_TLC5947.h>

#define DEBUG

#define transDelay 0
#define stepSize 585

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

uint16_t oldRedGoal;
uint16_t oldGreenGoal;
uint16_t oldBlueGoal;

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
	for(int i = 0; i < 4; i++){
		myLEDs[i].ledNum = i;
		myLEDs[i + 4].ledNum = i + 4;
		for(int j = 0; j < 3; j++){
			myLEDs[i].rgb[j] = 0;
			myLEDs[i].rgbGoal[j] = 0;
			myLEDs[i + 4].rgb[j] = 0;
			myLEDs[i + 4].rgbGoal[j] = 0;
		}

		switch(i){
			case 0:
			case 3:
				myLEDs[i].rgb[0] = 4095;
				myLEDs[i + 4].rgb[0] = 4095;

				myLEDs[i].rgbGoal[1] = 4095;
				myLEDs[i + 4].rgbGoal[1] = 4095;
				break;

			case 1:
				myLEDs[i].rgb[2] = 4095;
				myLEDs[i + 4].rgb[2] = 4095;

				myLEDs[i].rgbGoal[0] = 4095;
				myLEDs[i + 4].rgbGoal[0] = 4095;
				break;

			case 2:
				myLEDs[i].rgb[1] = 4095;
				myLEDs[i + 4].rgb[1] = 4095;

				myLEDs[i].rgbGoal[2] = 4095;
				myLEDs[i + 4].rgbGoal[2] = 4095;
				break;
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

		oldRedGoal = 0;
		oldGreenGoal = 0;
		oldBlueGoal = 0;
	}

	#ifdef DEBUG
		Serial.println("Post-initialization RGB values:");
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

		Serial.println("Post-initialization RGB goals:");
		for(int i = 0; i < 8; i++){
			Serial.print("REDGOAL");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgbGoal[0]);
			
			Serial.print("GREENGOAL");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgbGoal[1]);
		
			Serial.print("BLUEGOAL");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgbGoal[2]);
			Serial.println("");
		}
	#endif

	CCD.write();
}

void loop(){
	while(!goalAchieved(myLEDs)){
		for(int i = 0; i < 8; i++){
			fadeStep(myLEDs[i]);
		}
	}

	for(int i = 0; i < 8; i++){
		oldRedGoal = myLEDs[i].rgbGoal[0];
		oldGreenGoal = myLEDs[i].rgbGoal[1];
		oldBlueGoal = myLEDs[i].rgbGoal[2];
		myLEDs[i].rgbGoal[0] = oldBlueGoal;
		myLEDs[i].rgbGoal[1] = oldRedGoal;
		myLEDs[i].rgbGoal[2] = oldGreenGoal;
	}

	#ifdef DEBUG
		Serial.println("Next stage RGB values:");
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

		Serial.println("Next stage RGB goals:");
		for(int i = 0; i < 8; i++){
			Serial.print("REDGOAL");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgbGoal[0]);
			
			Serial.print("GREENGOAL");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgbGoal[1]);
		
			Serial.print("BLUEGOAL");
			Serial.print(i);
			Serial.print(": ");
			Serial.println(myLEDs[i].rgbGoal[2]);
			Serial.println("");
		}
	#endif
}


void fadeStep(struct led &myLED){
		if(myLED.rgb[0] < myLED.rgbGoal[0]){
			myLED.rgb[0] += stepSize;
		}
		else if(myLED.rgb[0] > myLED.rgbGoal[0]){
			myLED.rgb[0] -= stepSize;
		}

		if(myLED.rgb[1] < myLED.rgbGoal[1]){
			myLED.rgb[1] += stepSize;
		}
		else if(myLED.rgb[1] > myLED.rgbGoal[1]){
			myLED.rgb[1] -= stepSize;
		}

		if(myLED.rgb[2] < myLED.rgbGoal[2]){
			myLED.rgb[2] += stepSize;
		}
		else if(myLED.rgb[2] > myLED.rgbGoal[2]){
			myLED.rgb[2] -= stepSize;
		}

		#ifdef DEBUG
			Serial.println("Write values:");
			Serial.print("RED");
			Serial.print(myLED.ledNum);
			Serial.print(": ");
			Serial.println(myLED.rgb[0]);
			Serial.print("GREEN");
			Serial.print(myLED.ledNum);
			Serial.print(": ");
			Serial.println(myLED.rgb[1]);
			Serial.print("BLUE");
			Serial.print(myLED.ledNum);
			Serial.print(": ");
			Serial.println(myLED.rgb[2]);
			Serial.println("");
		#endif

		CCD.setLED(myLED.ledNum, myLED.rgb[0], myLED.rgb[1], myLED.rgb[2]);
		CCD.write();

		delay(transDelay);
}


bool goalAchieved(led myLEDs[]){
	for(int i = 0; i < 8; i++){
		if(myLEDs[i].rgb[0] != myLEDs[i].rgbGoal[0]){
			#ifdef DEBUG
				Serial.println("goalAchieved red check returned false");
			#endif;
			return false;
		}
		else if(myLEDs[i].rgb[1] != myLEDs[i].rgbGoal[1]){
			#ifdef DEBUG
				Serial.println("goalAchieved green check returned false");
			#endif;
			return false;
		}
		else if(myLEDs[i].rgb[2] != myLEDs[i].rgbGoal[2]){
			#ifdef DEBUG
				Serial.println("goalAchieved blue check returned false");
			#endif;
			return false;
		}
	}

	#ifdef DEBUG
		Serial.println("goalAchieved returned true");
	#endif;
	return true;
}
