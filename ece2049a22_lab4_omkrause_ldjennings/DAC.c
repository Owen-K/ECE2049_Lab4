/*
 * DAC.c
 *
 *  Created on: Oct 12, 2022
 *      Author: okrause
 */




#include "DAC.h"
#include "peripherals.h"

unsigned long int clock = 0;
unsigned int lastCode = 0;
enum DACMode lastMode = DAC_NONE;

void SetupDAC()
{
    setupSPI_DAC();

    DAC_PORT_LDAC_SEL &= ~DAC_PIN_LDAC;
    DAC_PORT_LDAC_DIR |= DAC_PIN_LDAC;
    DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC;

    DAC_PORT_CS_SEL &= ~DAC_PIN_CS;
    DAC_PORT_CS_DIR |= DAC_PIN_CS;
    DAC_PORT_CS_OUT |= DAC_PIN_CS;
}


void SetOutput(unsigned int amplitude_code)
{
    if (amplitude_code > maxCode)
        amplitude_code = maxCode;

    if (amplitude_code == lastCode)
        return;

    __no_operation();
    lastCode = amplitude_code;

    DAC_PORT_CS_OUT &= ~DAC_PIN_CS;

    amplitude_code |= 0x3000; // Add control bits to DAC word
    unsigned char low_byte = (unsigned char) (amplitude_code & 0x00FF);
    unsigned char high_byte = (unsigned char) ((amplitude_code & 0xFF00) >> 8);

    DAC_SPI_REG_TXBUF = high_byte;
    WaitForTransmission();
    DAC_SPI_REG_TXBUF = low_byte;
    WaitForTransmission();

    DAC_PORT_CS_OUT |= DAC_PIN_CS;
    ToggleLDAC();
}

void SetDC(unsigned int amplitude_code)
{
    if (lastMode != DAC_DC)
    {
        ConfigTimer(0);
        lastMode = DAC_DC;
    }
    SetOutput(amplitude_code);
}

void SetSquareWave(unsigned int amplitude_code)
{
    if (lastMode != DAC_SQUARE)
    {
        ConfigTimer(100 * 2);
        lastMode = DAC_SQUARE;
    }

    SetOutput(clock % 2 ? amplitude_code : 0);
}

void SetSawtoothWave()
{
    if (lastMode != DAC_SAWTOOTH)
    {
        ConfigTimer(50 * stepCount);
        lastMode = DAC_SAWTOOTH;
    }

//    float currentStep = clock % stepCount;

    SetOutput((unsigned int)((float)clock / stepCount * maxCode));
}

void SetTriangleWave(unsigned int frequency)
{
    if (lastMode != DAC_TRIANGLE)
    {
        ConfigTimer(frequency * stepCount);
        lastMode = DAC_TRIANGLE;
    }

    if (clock >= (stepCount/2))
        SetOutput((unsigned int)((((stepCount) - (clock)) * ((float)maxCode / stepCount))));
    else
        SetOutput((unsigned int)(((clock) * ((float)maxCode / stepCount))));
}

// Configure Timer A2, used to schedule changes in DAC output for square/sawtooth/triangle modes
void ConfigTimer(unsigned long int frequency)
{
    // If we're not using the timer, disable interrupts
    if (!frequency)
    {
        // We don't care how the timer is configured; just disable interrupts so we don't waste CPU time
        TA2CCTL0 &= ~CCIE; // Disable interrupt
        return;
    }

    TA2CTL = TASSEL_2 + ID_0 + MC_1; // SMCLK, no divider, up mode
    TA2CCR0 = (unsigned int) (1048676 / frequency); // SMCLK frequency / input frequency = timer ticks
    TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
}


#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer0_A2_ISR()
{
    clock++;
    if(clock == stepCount) clock = 0;
}

void WaitForTransmission(void)
{
    while (!(DAC_SPI_REG_IFG & UCTXIFG))
    {
        __no_operation();
    }
}

void ToggleLDAC(void)
{
    DAC_PORT_LDAC_OUT &= ~DAC_PIN_LDAC;
    __delay_cycles(10);
    DAC_PORT_LDAC_OUT |= DAC_PIN_LDAC;
}
