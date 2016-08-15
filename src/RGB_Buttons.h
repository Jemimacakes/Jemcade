// This file contains structs and enums for RGB_BUTTON_Arduino.ino //

// LED struct object for holding data relating to individual RGB channels on the TLC5947 //
typedef struct led{
	uint16_t ledNum;                                                        // LED channel number
	uint16_t rgb[3];                                                        // Current RGB values
	uint16_t rgbGoal[3];                                                    // Target RGB values
} led;

// Emumeration for possible stepSizes //
typedef enum Speed{
	S1 = 1,
	S2 = 3,
	S3 = 5,
	S4 = 7,
	S5 = 9,
	S6 = 13,
	S7 = 15,
	S8 = 21,
	S9 = 35,
	S10 = 39,
	S11 = 45,
	S12 = 63,
	S13 = 65,
	S14 = 91,
	S15 = 105,
	S16 = 117,
	S17 = 195,
	S18 = 273,
	S19 = 315,
	S20 = 455,
	S21 = 585,
	S22 = 819,
	S23 = 1365,
	S24 = 4095
} Speed;