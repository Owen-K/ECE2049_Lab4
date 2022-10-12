/*
 * DAC.h
 *
 *  Created on: Oct 12, 2022
 *      Author: okrause
 */

#ifndef DAC_H_
#define DAC_H_


enum DACMode {
    DAC_NONE,
    DAC_DC,
    DAC_SQUARE,
    DAC_SAWTOOTH,
    DAC_TRIANGLE
};


void SetupDAC();
void SetDC(unsigned int amplitude_code);
void SetSquareWave(unsigned int amplitude_code);
void SetSawtoothWave();
void SetTriangleWave(unsigned int frequency);
static void SetOutput(unsigned int amplitude_code);
static void WaitForTransmission(void);
static void ToggleLDAC(void);
void ConfigTimer(unsigned long int frequency);

static const unsigned int maxCode = 4095;
static const unsigned int stepCount = 20;

static void TimerISR(void);


#endif /* DAC_H_ */
