// General dependencies
#include <Arduino.h>
#include <SPI.h>

// Sensor shield 
#include "CN0391.h"
#include "Communication.h"
#include "Thermocouple.h"

//============ Functions ==============
#include "BasicCN0391.h"

// Declare the global ADC reading array and thermocouple type array from the CN0391 library
extern int32_t _ADCValue1[];
extern uint8_t th_types[NUM_PORT];

/*NOTE: Much of this code is from a modified sketch written by an unknown author.
Credit goes to whomever wrote it. 
*/

// Parser to isolate constants from "thermocouple.h"
int setType(char type) {
	switch(type) {
		case 'T':
			return TYPE_T;
			break;
		case 'J':
			return TYPE_J;
			break;
		case 'K':
			return TYPE_K;
			break;
		case 'E':
			return TYPE_E;
			break;
		case 'S':
			return TYPE_S;
			break;
		case 'R':
			return TYPE_R;
			break;
		case 'N':
			return TYPE_N;
			break;
		default: 				// Type B
			return TYPE_B;
	}
}

void CN0391_setup( char port[] ) { // vectorize to array
	// SPI setup
	SPI.begin();
	SPI.setDataMode(SPI_MODE3);
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);

	// Set all thermocouple channels to given inputs
	for( int ch = 0; ch < NUM_PORT; ch += 1 ) {
		th_types[ch] = setType( port[ch] );
	}

	// Calibrate the RTD (cold junction) and thermocouple channels
	CN0391_init();
	CN0391_calibration(RTD_CHANNEL);
	CN0391_calibration(TH_CHANNEL);
}

void CN0391_setup( char port ) {		// Over-ride for identical types
	char arr[] = {port, port, port, port};
	CN0391_setup(arr);
}

// get all temps
void CN0391_getTemps(float cjTemp[], float tcTemp[], float tcVoltage[], int32_t tcRaw[] ) {
	// Update all ADC data (both RTD and thermocouple channels)
	CN0391_set_data();

	for (int port = 0; port < NUM_PORT; port += 1) {
		// Get the cold junction temperature
		CN0391_calc_rtd_temperature(port, &cjTemp[port]);

		// Read the thermocouple raw ADC value
		tcRaw[port] = _ADCValue1[port];

		// Convert the raw ADC value to voltage
		tcVoltage[port] = CN0391_data_to_voltage(tcRaw[port], TH_CHANNEL);

		// Calculate the thermocouple temperature using the cold junction temperature
		CN0391_calc_th_temperature(port, cjTemp[port], &tcTemp[port]);
	}
}

// get probe temps
void CN0391_getThermocoupleTemps(float* tcTemp) {
	// Update all ADC data 
	CN0391_set_data();
	
	for (int port = 0; port < NUM_PORT; port += 1) {
		float cjTemp;
		CN0391_calc_rtd_temperature(port, &cjTemp);
		CN0391_calc_th_temperature(port, cjTemp, &tcTemp[port]); 	// return tcTemp
	}
}
