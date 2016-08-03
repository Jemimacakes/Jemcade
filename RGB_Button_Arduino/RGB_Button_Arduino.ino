#include <Adafruit_TLC5947.h>

//#define DEBUG

#define numBoards 1															// Number of linked TLC5947s

#define clk 4                                                               // Data clock
#define dout 5                                                              // Data output to CCD
#define lat 6                                                               // Data latch signal
#define oe_n -1                                                             // Data output enable


Adafruit_TLC5947 CCD(numBoards, clk, dout, lat);                            // Instantiate TLC5947 constant current driver

// LED struct object for holding data relating to individual RGB channels on the TLC5947 //
struct led{
	uint16_t ledNum;                                                        // LED channel number
	uint16_t rgb[3];                                                        // Current RGB values
	uint16_t rgbGoal[3];                                                    // Target RGB values
};
struct led* myLEDs;                                                         // Create pointer for dynamic array of LEDs

uint16_t oldRedGoal;                                                        // Old red goal value for rotating goals after transmission
uint16_t oldGreenGoal;                                                      // Old green goal value for rotating goals after transmission
uint16_t oldBlueGoal;                                                       // Old blue goal value for rotating goals after transmission

// Arduino setup() function //
void setup(){
	CCD.begin();                                                            // Start TLC5947 communications

	#ifdef DEBUG                                                            // If in debug mode
		Serial.begin(9600);                                                 // Start serial communications
		Serial.println("Begin program");                                    // Mark start of program
	#endif

	// Optional data output enable //
	if(oe_n >= 0){                                                          // If oe_n is not 0
		pinMode(oe_n, OUTPUT);                                              // Enable the pin as an output
		digitalWrite(oe_n, LOW);                                            // Hold it low
	}

	// Allocate and initialize LEDs - Initialize LEDs for a 2x4 arrangement where //
	// LEDs in the same column are identical //
	myLEDs = new led[numBoards * 8];                                        // Allocate memory for the LEDs 
	for(int i = 0; i < (numBoards * 8); i++){
		myLEDs[i].ledNum     = i;                                           // Set LED identification numbers
		for(int j = 0; j < 3; j++){
			myLEDs[i].rgb[j]         = 0;                                   // Initialize values to 0
			myLEDs[i].rgbGoal[j]     = 0;									// Initialize goals to 0
		}

		// Initialize each column's starting values and starting goals //
		switch(i % 8){
			case 0:                                                         // Columns 0 and 3 are the same
			case 3:
				myLEDs[i].rgb[0]         = 4095;                            // Start red
				myLEDs[i].rgbGoal[1]     = 4095;                            // Move to green
				break;

			case 1:
				myLEDs[i].rgb[2]         = 4095;                            // Start blue
				myLEDs[i].rgbGoal[0]     = 4095;                            // Move to red
				break;

			case 2:
				myLEDs[i].rgb[1]         = 4095;                            // Start green
				myLEDs[i].rgbGoal[2]     = 4095;                            // Move to blue
				break;
		}

		// Write initial values to TLC5947 //
		CCD.setLED(i, myLEDs[i].rgb[0], myLEDs[i].rgb[1], myLEDs[i].rgb[2]);

		#ifdef DEBUG                                                        // If in debug mode
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

	#ifdef DEBUG                                                            // If in debug mode
		// Print the LEDs initial values //
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

		// Print the LEDs initial goals //
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

	// Latch in new values and change LEDs //
	CCD.write();
}

// Arduino loop() function //
void loop(){
	flow(5, 0, 200, 195, "right");
}


// Smooth fade function, colors flow by fading in direction direction //
void flow(int numLoops, int transDelay, int holdDelay, int stepSize, String direction){
	// Set RGB goals for transition //
	for(int i = 0; i <= (numBoards * 8); i++){
		if(direction == "right"){
			switch(i % 8){
				case 0:                                                         // Columns 0 and 3 are the same
				case 3:
					myLEDs[i].rgbGoal[1]     = 4095;                            // Move to green
					break;
		
				case 1:
					myLEDs[i].rgbGoal[0]     = 4095;                            // Move to red
					break;
		
				case 2:
					myLEDs[i].rgbGoal[2]     = 4095;                            // Move to blue
					break;
			}
		}
		else if(direction == "left"){
			switch(i % 8){
				case 0:                                                         // Columns 0 and 3 are the same
				case 3:
					myLEDs[i].rgbGoal[2]     = 4095;                            // Move to blue
					break;
		
				case 1:
					myLEDs[i].rgbGoal[1]     = 4095;                            // Move to green
					break;
		
				case 2:
					myLEDs[i].rgbGoal[0]     = 4095;                            // Move to red
					break;
			}
		}
	}

	for(int i = 0; i < numLoops; i++){
		// While the goals are not reached, step towards them //
		while(!goalAchieved(myLEDs)){
			for(int i = 0; i < (numBoards * 8); i++){
				fadeStep(myLEDs[i], transDelay, stepSize);
			}
		}
	
		for(int i = 0; i < (numBoards * 8); i++){
			if(direction == "right"){
				oldRedGoal           = myLEDs[i].rgbGoal[0];
				oldGreenGoal         = myLEDs[i].rgbGoal[1];
				oldBlueGoal          = myLEDs[i].rgbGoal[2];
				myLEDs[i].rgbGoal[0] = oldBlueGoal;
				myLEDs[i].rgbGoal[1] = oldRedGoal;
				myLEDs[i].rgbGoal[2] = oldGreenGoal;
			}
			else if(direction == "left"){
				oldRedGoal           = myLEDs[i].rgbGoal[0];
				oldGreenGoal         = myLEDs[i].rgbGoal[1];
				oldBlueGoal          = myLEDs[i].rgbGoal[2];
				myLEDs[i].rgbGoal[0] = oldGreenGoal;
				myLEDs[i].rgbGoal[1] = oldBlueGoal;
				myLEDs[i].rgbGoal[2] = oldRedGoal;
			}
		}
	
		delay(holdDelay);
	
		#ifdef DEBUG                                                            // If in debug mode
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
}

// fadeStep() function takes and LED and adjusts its values by one stepSize //
void fadeStep(struct led &myLED, int transDelay, int stepSize){
		if(myLED.rgb[0] < myLED.rgbGoal[0]){
			myLED.rgb[0] += stepSize;                                       // Step red by one stepSize up if it is less than its goal
		}
		else if(myLED.rgb[0] > myLED.rgbGoal[0]){
			myLED.rgb[0] -= stepSize;                                       // Step red by one stepSize down if it is more than its goal
		}

		if(myLED.rgb[1] < myLED.rgbGoal[1]){
			myLED.rgb[1] += stepSize;                                       // Step green by one stepSize up if it is less than its goal
		}
		else if(myLED.rgb[1] > myLED.rgbGoal[1]){
			myLED.rgb[1] -= stepSize;                                       // Step green by one stepSize down if it is more than its goal
		}

		if(myLED.rgb[2] < myLED.rgbGoal[2]){
			myLED.rgb[2] += stepSize;                                       // Step blue by one stepSize up if it is less than its goal
		}
		else if(myLED.rgb[2] > myLED.rgbGoal[2]){
			myLED.rgb[2] -= stepSize;                                       // Step blue by one stepSize down if it is more than its goal
		}

		#ifdef DEBUG                                                        // If in debug mode
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

		CCD.setLED(myLED.ledNum, myLED.rgb[0], myLED.rgb[1], myLED.rgb[2]); // Write new values to the TLC5947
		CCD.write();                                                        // Latch in new values

		delay(transDelay);                                                  // Wait for the delay time
}

// goalAchieved() function returns a boolean representing if all of the LEDs are at //
// their goals //
bool goalAchieved(led myLEDs[]){
	// Check each LED for goals matching their values //
	for(int i = 0; i < 8; i++){
		if(myLEDs[i].rgb[0]      != myLEDs[i].rgbGoal[0]){                  // Check red value against its goal
			#ifdef DEBUG                                                    // If in debug mode
				// Print out confirmation of entering this if statment //
				Serial.println("goalAchieved red check returned false");
			#endif;
			return false;                                                   // return false if they are not equal
		}
		else if(myLEDs[i].rgb[1] != myLEDs[i].rgbGoal[1]){                  // Check red value against its goal
			#ifdef DEBUG                                                    // If in debug mode
				// Print out confirmation of entering this if statment //
				Serial.println("goalAchieved green check returned false");
			#endif;
			return false;                                                   // return false if they are not equal
		}
		else if(myLEDs[i].rgb[2] != myLEDs[i].rgbGoal[2]){                  // Check red value against its goal
			#ifdef DEBUG                                                    // If in debug mode
				// Print out confirmation of entering this if statment //
				Serial.println("goalAchieved blue check returned false");
			#endif;
			return false;                                                   // return false if they are not equal
		}
	}

	#ifdef DEBUG                                                            // If in debug mode
		// Print out confirmation that no if statment was entered //
		Serial.println("goalAchieved returned true");
	#endif;
	return true;                                                            // Return true of they are all equal
}
