#ifndef _BasicCN0391_H_
#define _BasicCN0391_H_

constexpr uint8_t NUM_PORT = 4; // Global constant | Number of ports available in CN0391

// Parser
int setType( char );

// initialize module
void CN0391_setup( char[] );
void CN0391_setup( char );

// get temps
void CN0391_getTemps(float [], float [], float [], int32_t [] );
void CN0391_getThermocoupleTemps(float*);

#endif
