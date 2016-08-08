#include <Adafruit_TLC5947.h>
#include "RGB_Button_Arduino.h"

//#define DEBUG

#define numBoards 1                                                             // Number of linked TLC5947s

#define clk 4                                                                   // Data clock
#define dout 5                                                                  // Data output to CCD
#define lat 6                                                                   // Data latch signal
#define oe_n -1                                                                 // Data output enable


Adafruit_TLC5947 CCD(numBoards, clk, dout, lat);                                // Instantiate TLC5947 constant current driver

led* myLEDs;                                                                    // Create pointer for dynamic array of LEDs

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

	// Allocate and initialize LEDs - Initialize LEDs for a 2x4 arrangement where //
	// LEDs in the same column are identical //
	myLEDs = new led[numBoards * 8];                                            // Allocate memory for the LEDs 
	for(int i = 0; i < (numBoards * 8); i++){
		myLEDs[i].ledNum     = i;                                               // Set LED identification numbers
		for(int j = 0; j < 3; j++){
			myLEDs[i].rgb[j]         = 0;                                       // Initialize values to 0
			myLEDs[i].rgbGoal[j]     = 0;                                       // Initialize goals to 0
		}

		// Initialize each column's starting values //
		switch(i % 4){
			case 0:                                                             // Columns 0 and 3 are the same
			case 3:
				myLEDs[i].rgb[0]         = 4095;                                // Start red
				break;

			case 1:
				myLEDs[i].rgb[2]         = 4095;                                // Start blue
				break;

			case 2:
				myLEDs[i].rgb[1]         = 4095;                                // Start green
				break;
		}

		// Write initial values to TLC5947 //
		CCD.setLED(i, myLEDs[i].rgb[0], myLEDs[i].rgb[1], myLEDs[i].rgb[2]);

		#ifdef DEBUG                                                            // If in debug mode
			// Mark each loops with confirmation printout //
			Serial.print("LED ");
			Serial.print(i);
			Serial.println(" initialized\n");
		#endif

		// Initialize temporary containers //
		oldRedGoal   = 0;
		oldGreenGoal = 0;
		oldBlueGoal  = 0;
	}

	#ifdef DEBUG                                                                // If in debug mode
		// Print the LEDs initial values //
		Serial.println("Post-initialization RGB values:");
		for(int i = 0; i < (numBoards * 8); i++){
			Serial.print("LED Number: ");
			Serial.println(myLEDs[i].ledNum);
			
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
	// Set RGB starting goals for transition //
	for(int i = 0; i <= (numBoards * 8); i++){
		if(direction == "right"){
			switch(i % 4){
				case 0:                                                         // Columns 0 and 3 are the same
				case 3:
					myLEDs[i].rgbGoal[0]     = 0;
					myLEDs[i].rgbGoal[1]     = 4095;                            // Move to green
					myLEDs[i].rgbGoal[2]     = 0;
					break;
		
				case 1:
					myLEDs[i].rgbGoal[0]     = 4095;                            // Move to red
					myLEDs[i].rgbGoal[1]     = 0;
					myLEDs[i].rgbGoal[2]     = 0;
					break;
		
				case 2:
					myLEDs[i].rgbGoal[0]     = 0;
					myLEDs[i].rgbGoal[1]     = 0;
					myLEDs[i].rgbGoal[2]     = 4095;                            // Move to blue
					break;
			}
		}
		else if(direction == "left"){
			switch(i % 4){
				case 0:                                                         // Columns 0 and 3 are the same
				case 3:
					myLEDs[i].rgbGoal[0]     = 0;
					myLEDs[i].rgbGoal[1]     = 0;
					myLEDs[i].rgbGoal[2]     = 4095;                            // Move to blue
					break;
		
				case 1:
					myLEDs[i].rgbGoal[0]     = 0;
					myLEDs[i].rgbGoal[1]     = 4095;                            // Move to green
					myLEDs[i].rgbGoal[2]     = 0;
					break;
		
				case 2:
					myLEDs[i].rgbGoal[0]     = 4095;                            // Move to red
					myLEDs[i].rgbGoal[1]     = 0;
					myLEDs[i].rgbGoal[2]     = 0;
					break;
			}
		}
	}


	for(int i = 0; i < numLoops; i++){
		#ifdef DEBUG
			Serial.print("flow() loop ");
			Serial.print(i);
			Serial.println(":");
		#endif
		
		// Loop for an entire cycle //
		for(int j = 0; j < 3; j++){
			#ifdef DEBUG                                                        // If in debug mode
				Serial.println("This iteration's RGB values:");
				for(int i = 0; i < (numBoards * 8); i++){
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
		
				Serial.println("This iteration's RGB goals:");
				for(int i = 0; i < (numBoards * 8); i++){
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

			// While the goals are not reached, step towards them //
			while(!goalAchieved(myLEDs)){
				for(int k = 0; k < (numBoards * 8); k++){
					fadeStep(myLEDs[k], transDelay, speed);
				}
			}

			for(int k = 0; k < (numBoards * 8); k++){
				if(direction == "right"){
					oldRedGoal           = myLEDs[k].rgbGoal[0];
					oldGreenGoal         = myLEDs[k].rgbGoal[1];
					oldBlueGoal          = myLEDs[k].rgbGoal[2];
					myLEDs[k].rgbGoal[0] = oldBlueGoal;
					myLEDs[k].rgbGoal[1] = oldRedGoal;
					myLEDs[k].rgbGoal[2] = oldGreenGoal;
				}
				else if(direction == "left"){
					oldRedGoal           = myLEDs[k].rgbGoal[0];
					oldGreenGoal         = myLEDs[k].rgbGoal[1];
					oldBlueGoal          = myLEDs[k].rgbGoal[2];
					myLEDs[k].rgbGoal[0] = oldGreenGoal;
					myLEDs[k].rgbGoal[1] = oldBlueGoal;
					myLEDs[k].rgbGoal[2] = oldRedGoal;
				}
			}

			delay(holdDelay);
		}
	}
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
Inputs: myLEDs - Array of led objects to check.
Outputs: boolean.
Description: Checks RGB values vs RGB goals and returns a
			 boolean representing whether or not all the values
			 match the goals.
*****************************************************************/
bool goalAchieved(struct led myLEDs[]){
	// Check each LED for goals matching their values //
	for(int i = 0; i < (numBoards * 8); i++){
		if(myLEDs[i].rgb[0]      != myLEDs[i].rgbGoal[0]){                      // Check red value against its goal
			#ifdef DEBUG                                                        // If in debug mode
				// Print out confirmation of entering this if statment //
				Serial.println("goalAchieved red check returned false");
			#endif;
			return false;                                                       // return false if they are not equal
		}
		else if(myLEDs[i].rgb[1] != myLEDs[i].rgbGoal[1]){                      // Check red value against its goal
			#ifdef DEBUG                                                        // If in debug mode
				// Print out confirmation of entering this if statment //
				Serial.println("goalAchieved green check returned false");
			#endif;
			return false;                                                       // return false if they are not equal
		}
		else if(myLEDs[i].rgb[2] != myLEDs[i].rgbGoal[2]){                      // Check red value against its goal
			#ifdef DEBUG                                                        // If in debug mode
				// Print out confirmation of entering this if statment //
				Serial.println("goalAchieved blue check returned false");
			#endif;
			return false;                                                       // return false if they are not equal
		}
	}

	#ifdef DEBUG                                                                // If in debug mode
		// Print out confirmation that no if statment was entered //
		Serial.println("goalAchieved returned true");
	#endif;
	return true;                                                                // Return true of they are all equal
}
