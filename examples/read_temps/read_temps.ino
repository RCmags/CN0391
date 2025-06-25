#include <SPI.h>
#include "CN0391.h"
#include "Communication.h"
#include "Thermocouple.h"  // For TYPE_K

/* NOTE:
	This sketch has an unknown author. Credit goes to whomever wrote it. 
*/

// Declare the global ADC reading array and thermocouple type array from the CN0391 library
extern int32_t _ADCValue1[];
extern uint8_t th_types[4];

void setup() {
	Serial.begin(9600);
	SPI.begin();
	SPI.setDataMode(SPI_MODE3);
	pinMode(CS_PIN, OUTPUT);
	digitalWrite(CS_PIN, HIGH);

	// Initialize the CN0391 board (sets up both RTD and thermocouple channels)
	CN0391_init();

	// Calibrate the RTD (cold junction) and thermocouple channels
	CN0391_calibration(RTD_CHANNEL);
	CN0391_calibration(TH_CHANNEL);

	// Set all thermocouple channels to K-type
	th_types[0] = TYPE_K;
	th_types[1] = TYPE_K;
	th_types[2] = TYPE_K;
	th_types[3] = TYPE_K;

	Serial.println(F("CN0391 Thermocouple Temperature Readings:"));
	Serial.println(F("----------------------------------------"));
}

void loop() {
	delay(10000);  // 1 second delay between readings

	// Update all ADC data (both RTD and thermocouple channels)
	CN0391_set_data();

	// Arrays to hold readings for 4 ports
	float cjTemp[4] = { 0, 0, 0, 0 };
	float tcTemp[4] = { 0, 0, 0, 0 };
	float tcVoltage[4] = { 0, 0, 0, 0 };
	int32_t tcRaw[4] = { 0, 0, 0, 0 };

	// For each port (P1 to P4) update readings:
	for (int i = 0; i < 4; i++) {
		// Get the cold junction temperature using the RTD channel corresponding to port i
		CN0391_calc_rtd_temperature(i, &cjTemp[i]);

		// Read the thermocouple raw ADC value from the global array (_ADCValue1)
		tcRaw[i] = _ADCValue1[i];

		// Convert the raw ADC value to voltage using the TH_CHANNEL configuration
		tcVoltage[i] = CN0391_data_to_voltage(tcRaw[i], TH_CHANNEL);

		// Calculate the thermocouple temperature using the cold junction temperature
		CN0391_calc_th_temperature(i, cjTemp[i], &tcTemp[i]);
	}

	// Print the results for each port in the desired format
	for (int i = 0; i < 4; i++) {
		Serial.print(F("Cold Junction Temp (P"));
		Serial.print(i + 1);
		Serial.print(F("): "));
		Serial.print(cjTemp[i]);
		Serial.println(F(" °C"));

		Serial.print(F("Thermocouple Raw ADC (P"));
		Serial.print(i + 1);
		Serial.print(F("): "));
		Serial.print(tcRaw[i]);
		Serial.print(F("\tVoltage: "));
		Serial.print(tcVoltage[i], 6);
		Serial.println(F(" mV"));

		Serial.print(F("Thermocouple Temp (P"));
		Serial.print(i + 1);
		Serial.print(F("): "));
		Serial.print(tcTemp[i]);
		Serial.println(F(" °C"));
		Serial.println(F("-----------"));
	}
}
