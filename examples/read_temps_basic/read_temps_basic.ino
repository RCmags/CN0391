#include "BasicCN0391.h"  

void setup() {
	// Setup for equal sensor types
	//CN0391_setup('N'); // use Type N sensors

	// Setup for different sensor types
	char types[] = {'N', 'N', 'N', 'N'};
	CN0391_setup(types);

	Serial.begin(9600);
}

void loop() {
	// get data;
	float tcTemp[4]; 
	CN0391_getThermocoupleTemps(tcTemp);

	// display
	for ( int i = 0; i < 4; i +=1 ) {
		Serial.print(tcTemp[i]);
		Serial.print(" ");
	}
	Serial.println();
}

