#include <Adafruit_TLC5947.h>
#include "RGB_Button_Arduino.h"

//#define DEBUG

#define numBoards 1                                                             // Number of linked TLC5947s
#define rows 2																	// Number of rows on the CCD
#define cols 4																	// Number of cols on the CCD

#define clk 4                                                                   // Data clock
#define dout 5                                                                  // Data output to CCD
#define lat 6                                                                   // Data latch signal
#define oe_n -1                                                                 // Data output enable


Adafruit_TLC5947 CCD(numBoards, clk, dout, lat);                                // Instantiate TLC5947 constant current driver

led myLEDs[numBoards][rows][cols];                                              // Create pointer for dynamic array of LEDs

uint16_t oldRedGoal;                                                            // Old red goal value for rotating goals after transmission
uint16_t oldGreenGoal;                                                          // Old green goal value for rotating goals after transmission
uint16_t oldBlueGoal;                                                           // Old blue goal value for rotating goals after transmission

/****************************************************************
Name: setup()
Inputs: none.
Outputs: none.
Description: Standard Arduino setup function. Initializes values
			 and calls begin for features.
*****************************************************************/
void setup(){
	CCD.begin();                                                                // Start TLC5947 communications

	#ifdef DEBUG                                                                // If in debug mode
		Serial.begin(9600);                                                     // Start serial communications
		Serial.println("Begin program");                                        // Mark start of program
	#endif

	// Optional data output enable //
	if(oe_n >= 0){                                                              // If oe_n is not 0
		pinMode(oe_n, OUTPUT);                                                  // Enable the pin as an output
		digitalWrite(oe_n, LOW);                                                // Hold it low
	}

	// Initialize LEDs //
	for(int i = 0; i < numBoards; i++){
		for(int j = 0; j < rows; j++){
			for(int k = 0; k < cols; k++){
				/******************************************* 
					Apply this numbering scheme to the LEDs:
				    7  6  5  4		15 14 13 12
					0  1  2  3		 8  9 10 11
				********************************************/
				if(j % 2 == 0){
					myLEDs[i][j][k].ledNum = (8 * (i + 1)) - (1 + k);
				}
				else{
					myLEDs[i][j][k].ledNum = (8 * i) + k;
				}

				// Clear all values and goals //
				for(int x = 0; x < 3; x++){
					myLEDs[i][j][k].rgb[x] = 0;
					myLEDs[i][j][k].rgbGoal[x] = 0;
				}

				#ifdef DEBUG
					Serial.print("LED ");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.println(" initialized!");

					Serial.print("RED");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgb[0]);

					Serial.print("GREEN");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgb[1]);

					Serial.print("BLUE");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgb[2]);

					Serial.println();
				#endif
			}
		}
	}

	// Initialize temporary containers //
	oldRedGoal   = 0;
	oldGreenGoal = 0;
	oldBlueGoal  = 0;

	// Latch in new values and change LEDs //
	CCD.write();
}

/****************************************************************
Name: loop()
Inputs: none.
Outputs: none.
Description: Standard Arduino loop function. Loops indefinitely.
*****************************************************************/
void loop(){
	flow(5, 0, 0, S20, "right");
	flow(5, 0, 0, S20, "left");

	#ifdef DEBUG
		while(1){};
	#endif
}


/****************************************************************
Name: flow()
Inputs: numLoops - Integer representing how many times to loop.
		transDelay - Integer representing how long to delay
					 between steps.
		holdDelay - Integer representing how long to hold the
					goal values.
		speed - Speed object representing stepSize to pass to
				fadeStep.
				Possible values: S1 - S24
		direction - String representing which directin to flow.
					Possible values:
						- "right": flow right.
						- "left": flow left.
Outputs: none
Description: flow function fades the LEDs down the line like the
			 colors are flowing in the direction specified.
*****************************************************************/
void flow(int numLoops, int transDelay, int holdDelay, Speed speed, String direction){
	// For each LED intialize the values to prevent overflow from //
	// previous functions. //
	for(int i = 0; i < numBoards; i++){
		for(int j = 0; j < rows; j++){
			for(int k = 0; k < cols; k++){
				// Clear values //
				for(int x = 0; x < 3; x++){
					myLEDs[i][j][k].rgb[x] = 0;
					myLEDs[i][j][k].rgbGoal[x]= 0;
				}

				// If we are flowing right //
				if(direction == "right"){
					// and the LEDs board number is even //
					if(i % 2 == 0){
						switch(k){
							case 0:                                             // and the LED is in column 0 or 3
							case 3:
								myLEDs[i][j][k].rgb[0] = 4095;                  // Set LED red
								myLEDs[i][j][k].rgbGoal[1] = 4095;              // Turn green
								break;
		
							case 1:                                             // and the LED is in column 1
								myLEDs[i][j][k].rgb[2] = 4095;                  // Set LED blue
								myLEDs[i][j][k].rgbGoal[0] = 4095;              // Turn red
								break;
		
							case 2:                                             // and the LED is in column 2
								myLEDs[i][j][k].rgb[1] = 4095;                  // Set LED green
								myLEDs[i][j][k].rgbGoal[2] = 4095;              // Turn blue
								break;
						}
					}
					// and the LEDs board number is odd //
					else{
						switch(k){
							case 0:                                             // and the LED is in column 0 or 3
							case 3:
								myLEDs[i][j][k].rgb[2] = 4095;                  // Set the LED blue
								myLEDs[i][j][k].rgbGoal[0] = 4095;              // Turn red
								break;
		
							case 1:                                             // and the LED is in column 1
								myLEDs[i][j][k].rgb[1] = 4095;                  // Set the LED green
								myLEDs[i][j][k].rgbGoal[2] = 4095;              // Turn blue
								break;
		
							case 2:                                             // and the LED is in column 2
								myLEDs[i][j][k].rgb[0] = 4095;                  // Set the LED red
								myLEDs[i][j][k].rgbGoal[1] = 4095;              // Turn green
								break;
						}
					}
				}
				// If we are flowing left //
				else if(direction == "left"){
					// and the LEDs board number is even //
					if(i % 2 == 0){
						switch(k){
							case 0:                                             // and the LED is in column 0 or 3
							case 3:
								myLEDs[i][j][k].rgb[0] = 4095;                  // Set the LED red
								myLEDs[i][j][k].rgbGoal[2] = 4095;              // Turn blue
								break;
		
							case 1:                                             // and the LED is in column 1
								myLEDs[i][j][k].rgb[2] = 4095;                  // Set the LED blue
								myLEDs[i][j][k].rgbGoal[1] = 4095;              // Turn green
								break;
		
							case 2:                                             // and the LED is in column 2
								myLEDs[i][j][k].rgb[1] = 4095;                  // Set the LED green
								myLEDs[i][j][k].rgbGoal[0] = 4095;              // Turn red
								break;
						}
					}
					// and the LEDs board number is odd //
					else{
						switch(k){
							case 0:                                             // and the LED is in column 0 or 3
							case 3:
								myLEDs[i][j][k].rgb[2] = 4095;                  // Set the LED blue
								myLEDs[i][j][k].rgbGoal[1] = 4095;              // Turn green
								break;
		
							case 1:                                             // and the LED is in column 1
								myLEDs[i][j][k].rgb[1] = 4095;                  // Set the LED green
								myLEDs[i][j][k].rgbGoal[0] = 4095;              // Turn red
								break;
		
							case 2:                                             // and the LED is in column 2
								myLEDs[i][j][k].rgb[0] = 4095;                  // Set the LED red
								myLEDs[i][j][k].rgbGoal[2] = 4095;              // Turn blue
								break;
						}
					}
				}

				CCD.setLED(myLEDs[i][j][k].ledNum, myLEDs[i][j][k].rgb[0],      // Set LED RGB values in TLC5947
						   myLEDs[i][j][k].rgb[1], myLEDs[i][j][k].rgb[2]);


				#ifdef DEBUG                                                    // If in DEBUG mode
					Serial.print("LED ");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.println("'s flow() starting values set!");

					Serial.print("RED");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgb[0]);

					Serial.print("GREEN");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgb[1]);

					Serial.print("BLUE");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgb[2]);

					Serial.print("REDGOAL");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgbGoal[0]);

					Serial.print("GREENGOAL");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgbGoal[1]);

					Serial.print("BLUEGOAL");
					Serial.print(myLEDs[i][j][k].ledNum);
					Serial.print(": ");
					Serial.println(myLEDs[i][j][k].rgbGoal[2]);

					Serial.println();
				#endif
			}
		}
	}
	CCD.write();                                                                // Write initial colors

	// Loop numLoops times //
	for(int x = 0; x < numLoops; x++){
		#ifdef DEBUG                                                            // if in DEBUG mode
			Serial.print("Loop ");
			Serial.print(x);
			Serial.println("begin:");
		#endif 

		// Loop through an entire cycle //
		for(int y = 0; y < 3; y++){
			while(!goalAchieved()){
				for(int i = 0; i < numBoards; i++){
					for(int j = 0; j < rows; j++){
						for(int k = 0; k < cols; k++){
							fadeStep(myLEDs[i][j][k], transDelay, speed);
						}
					}
				}
			}

			// For each LED, swap the goals to simulate "flow" //
			for(int i = 0; i < numBoards; i++){
				for(int j = 0; j < rows; j++){
					for(int k = 0; k < cols; k++){
						// If we are flowing right
						if(direction == "right"){
							oldRedGoal = myLEDs[i][j][k].rgbGoal[0];
							oldGreenGoal = myLEDs[i][j][k].rgbGoal[1];
							oldBlueGoal = myLEDs[i][j][k].rgbGoal[2];
							myLEDs[i][j][k].rgbGoal[0] = oldBlueGoal;           // If it was blue turn red
							myLEDs[i][j][k].rgbGoal[1] = oldRedGoal;            // If it was red turn green
							myLEDs[i][j][k].rgbGoal[2] = oldGreenGoal;          // If it was green turn blue
						}
						// If we are flowing left
						else if(direction == "left"){
							oldRedGoal = myLEDs[i][j][k].rgbGoal[0];
							oldGreenGoal = myLEDs[i][j][k].rgbGoal[1];
							oldBlueGoal = myLEDs[i][j][k].rgbGoal[2];
							myLEDs[i][j][k].rgbGoal[0] = oldGreenGoal;          // If it green turn red
							myLEDs[i][j][k].rgbGoal[1] = oldBlueGoal;           // If it was blue turn green
							myLEDs[i][j][k].rgbGoal[2] = oldRedGoal;            // If it was red turn blue
						}
					}
				}
			}

			delay(holdDelay);													// Delay before starting next color change
		}
	}
}


/****************************************************************
Name: zigZag()
Inputs: numLoops - Integer representing how many times to loop.
		transDelay - Integer representing how long to delay
					 between steps.
		holdDelay - Integer representing how long to hold the
					goal values.
		speed - Speed object representing stepSize to pass to
				fadeStep.
				Possible values: S1 - S24
Outputs: none
Description: zigZag draws a zig zag and swaps its orientation
			 while changing colors.
*****************************************************************/
void zigZag(int numLoops, int transDelay, int holdDelay, Speed speed){

}


/****************************************************************
Name: fadeStep()
Inputs: myLED - led object for adjustment. Pass by reference.
		transDelay - Integer representing how long to delay
					 between steps.
		stepSize - How big of a step to make. This must be
				   a factor of 4095.
Outputs: none.
Description: Adjusts RGB values by one stepSize toward its goal
			 value.
*****************************************************************/
void fadeStep(struct led &myLED, int transDelay, int stepSize){
		if(myLED.rgb[0] < myLED.rgbGoal[0]){
			myLED.rgb[0] += stepSize;                                           // Step red by one stepSize up if it is less than its goal
		}
		else if(myLED.rgb[0] > myLED.rgbGoal[0]){
			myLED.rgb[0] -= stepSize;                                           // Step red by one stepSize down if it is more than its goal
		}

		if(myLED.rgb[1] < myLED.rgbGoal[1]){
			myLED.rgb[1] += stepSize;                                           // Step green by one stepSize up if it is less than its goal
		}
		else if(myLED.rgb[1] > myLED.rgbGoal[1]){
			myLED.rgb[1] -= stepSize;                                           // Step green by one stepSize down if it is more than its goal
		}

		if(myLED.rgb[2] < myLED.rgbGoal[2]){
			myLED.rgb[2] += stepSize;                                           // Step blue by one stepSize up if it is less than its goal
		}
		else if(myLED.rgb[2] > myLED.rgbGoal[2]){
			myLED.rgb[2] -= stepSize;                                           // Step blue by one stepSize down if it is more than its goal
		}

		#ifdef DEBUG                                                            // If in debug mode
			// Print out new RGB values written to TLC5947 //
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

		CCD.setLED(myLED.ledNum, myLED.rgb[0], myLED.rgb[1], myLED.rgb[2]);     // Write new values to the TLC5947
		CCD.write();                                                            // Latch in new values

		delay(transDelay);                                                      // Wait for the delay time
}


/****************************************************************
Name: goalAchieved()
Inputs: none
Outputs: boolean.
Description: Checks RGB values vs RGB goals and returns a
			 boolean representing whether or not all the values
			 match the goals.
*****************************************************************/
bool goalAchieved(){
	for(int i = 0; i < numBoards; i++){
		for(int j = 0; j < rows; j++){
			for(int k = 0; k < cols; k++){
				if(myLEDs[i][j][k].rgb[0] != myLEDs[i][j][k].rgbGoal[0]){
					#ifdef DEBUG
						Serial.println("goalAchieved() red check returned false");
					#endif
					return false;
				}
				else if(myLEDs[i][j][k].rgb[1] != myLEDs[i][j][k].rgbGoal[1]){
					#ifdef DEBUG
						Serial.println("goalAchieved() green check returned false");
					#endif
					return false;
				}
				else if(myLEDs[i][j][k].rgb[2] != myLEDs[i][j][k].rgbGoal[2]){
					#ifdef DEBUG
						Serial.println("goalAchieved() blue check returned false");
					#endif
					return false;
				}
			}
		}
	}

	#ifdef DEBUG
		Serial.println("goalAchieved() returned true");
	#endif
	return true;
}
