#include <msp430.h> 
#include "peripherals.h"
#include "DAC.h"

/**
 * main.c
 */

// States utilized in main program state machine
enum State
{
    WELCOME_SCREEN, WAIT_FOR_SELECTION, DC, SQUARE, SAWTOOTH, TRIANGLE, TEST
};

// Function prototypes
void drawWelcome();



int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	_BIS_SR(GIE);   // Global interrupt enable
	enum State state = WELCOME_SCREEN;


	configDisplay();
	configButtons();
	setupADC12Scroll();
	SetupDAC();

	unsigned int pot;
	unsigned char buttons;

	while (1)
	    {
	        buttons = getButtons();
	        pot = getScrollVal();

	        if (buttons & BIT0)
	        {
	            state = DC;
	        }
	        else if (buttons & BIT1)
	        {
	            state = SQUARE;
	        }
	        else if (buttons & BIT2)
	        {
	            state = SAWTOOTH;
	        }
	        else if (buttons & BIT3)
	        {
	            state = TRIANGLE;
	        }

	        switch (state)
	        {
	        case WELCOME_SCREEN:
	            drawWelcome();
	            state = WAIT_FOR_SELECTION;
	            break;
	        case WAIT_FOR_SELECTION:
	            __no_operation();
	            break;
	        case DC:
	            // Display DC value starting at 0v to VCC
	            SetDC(pot);
	            break;
	        case SQUARE:
	            // Display square wave going from 0v to V_CC with 150Hz
	            SetSquareWave(pot);
	            break;
	        case SAWTOOTH:
	            // Display sawtooth wave going from 0v to V_CC with 75Hz
	            SetSawtoothWave();
	            break;
	        case TRIANGLE:
	            // Display triangle wave going from 0v to V_CC with 150Hz
	            //SetTriangleWave(100 + (unsigned int)((pot * 1000) / 4095.0)); // Frequency ranges from 100Hz at pot = 0 to 1kHz when pot = 4095
	            SetTriangleWave(50);
	            break;
	        }
	    }

	return 0;
}


// Draw Welcome screen
void drawWelcome() {
// *** Intro Screen ***
    Graphics_clearDisplay(&g_sContext);                // Clear the display

// Write some text to the display
    Graphics_drawStringCentered(&g_sContext, "MSP430", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "Function Gen", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);

    Graphics_drawString(&g_sContext, "B1: DC", AUTO_STRING_LENGTH, 10, 48, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "B2: Square", AUTO_STRING_LENGTH, 10, 58, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "B3: Sawtooth", AUTO_STRING_LENGTH, 10, 68, TRANSPARENT_TEXT);
    Graphics_drawString(&g_sContext, "B4: Triangle", AUTO_STRING_LENGTH, 10, 78, TRANSPARENT_TEXT);


    Graphics_flushBuffer(&g_sContext);
}
