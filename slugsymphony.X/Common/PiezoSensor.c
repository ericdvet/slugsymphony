/* 
 * File:   PiezoSensor.c
 * Author: erikv
 *
 * Created on January 17, 2024, 1:53 PM
 */

#include <stdio.h>
#include <stdlib.h>

#include "BOARD.h"
#include "SERIAL.h"
#include "AD.h"
#include "Oled.h"
#include "ToneGeneration.h"

#define TRUE 1
#define FALSE 0

#define KEYBOARD_TIME 20

/*
 * 
 */
int main(void) {

    BOARD_Init();
    AD_Init();
    ToneGeneration_Init();
    ToneGeneration_ToneOff();

    char keyboardFlag = TRUE;
    int keyboardFlagCounter = 0;

    // Initializes adc
    AD_AddPins(AD_A1);
    AD_AddPins(AD_A2);
    unsigned int piezoReading = AD_ReadADPin(AD_A1);
    unsigned int flexReading = AD_ReadADPin(AD_A2);
    unsigned int tone;

    // Stores potentiometer value in output
    char output[40];
    sprintf(output, "Flex: %d        \nPiezo:     %d"
            , flexReading, piezoReading);

    // Initializes Oled and output potentiometer value and percentage
    OledInit();
    OledDrawString(output);
    OledUpdate();

    while (1) {

        piezoReading = AD_ReadADPin(AD_A1);
        flexReading = AD_ReadADPin(AD_A2);
        sprintf(output, "Flex: %d        \nPiezo: %d    "
                , flexReading, piezoReading);
        OledDrawString(output);
        OledUpdate();

        if (AD_ReadADPin(AD_A1) > 50) {

            if (!keyboardFlag) {
                keyboardFlag = TRUE;
                tone = (int) (0.00113172162037950 * (float) flexReading * (float) flexReading
                        + 1.38922813051449 * (float) flexReading
                        - 1474.946792888927);
            }

            keyboardFlagCounter = 0;
        }

        if (keyboardFlagCounter > KEYBOARD_TIME) {
            keyboardFlag = FALSE;
            ToneGeneration_ToneOff();
        }

        if (keyboardFlag) {
            ToneGeneration_ToneOn();
            ToneGeneration_SetFrequency(tone);
            keyboardFlagCounter++;
        }

    }

    while (1);
}