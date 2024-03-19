#include "flex.h"
#include "Oled.h"
#include <stdio.h>

#define L_PINKYRING_DOWN 400 //A0
#define L_MIDPOINT_DOWN 400 //A1
#define R_MIDPOINT_DOWN 400 //A2
#define R_PINKYRING_DOWN 400 //A3

///need different flex values for each instrument. do this once they are attached and can work. 


#define DELAY(x)    {int stop; for (stop = 0; stop <= x; stop++) {asm("nop");}}
#define A_LOT      183000
#define DELAY_GOOD       183000 
#define DELAY_BETTER      18300

void FlexInit(void) {
    AD_Init();
    AD_AddPins(AD_A1);
    AD_AddPins(AD_A2);
    AD_AddPins(AD_A3);
    AD_AddPins(AD_A0);
}

int AllFlexCheck(void) {
    if (AD_ReadADPin(AD_A0) < L_PINKYRING_DOWN) {
        if (AD_ReadADPin(AD_A1) < L_MIDPOINT_DOWN) {
            if (AD_ReadADPin(AD_A2) < R_MIDPOINT_DOWN) {
                if (AD_ReadADPin(AD_A3) < R_PINKYRING_DOWN) {
                    return 1;
                }
            }
        }
    }
    return 0;
}

uint8_t FlexState(uint8_t flexStates) {
//    uint8_t flexStates = 0b000000;
    //    FlexInit();
    unsigned int Pin1 = AD_ReadADPin(AD_A1);
    unsigned int Pin2 = AD_ReadADPin(AD_A2);
    unsigned int Pin3 = AD_ReadADPin(AD_A3);
    unsigned int Pin4 = AD_ReadADPin(AD_A0);
    
    if (Pin1 <= LEFT_FINGER_ONE) {
        flexStates |= 0b100000;
    } else {
        flexStates &= 0b011111;
    }
    
    if (Pin2 <= LEFT_FINGER_TWO_THREE) {
        flexStates |= 0b010000;
    } else {
        flexStates &= 0b101111;
    }
    
    if (Pin2 <= LEFT_FINGER_THREE) {
        flexStates |= 0b001000;
    } else {
        flexStates &= 0b110111;
    }

    if (Pin3 <= RIGHT_FINGER_ONE_TWO) {
        flexStates |= 0b000010;
    } else if (Pin3 <= RIGHT_FINGER_ONE) {
        flexStates |= 0b000100;
    } else if (Pin3 <= RIGHT_FINGER_TWO) {
        flexStates |= 0b000010;
    } else {
        flexStates &= 0b111001;
    }
    
    if (Pin4 <= RIGHT_FINGER_THREE) {
        flexStates |= 0b000000;
    } else {
        flexStates &= 0b111110;
    }

    //    if (Pin0 <= LEFT_PINKY_RING_AD) {
    //        flexStates |= 0b11000000;
    //    } else if (Pin0 > LEFT_PINKY_RING_AD && Pin0 <= LEFT_PINKY_AD) {
    //        flexStates |= 0b10000000;
    //    } else if (Pin0 > LEFT_PINKY_AD && Pin0 <= LEFT_RING_AD) {
    //        flexStates |= 0b01000000;
    //    }

    //    if (Pin1 <= LEFT_MID_POINT_AD) {
    //        flexStates |= 0b00110000;
    //    } else if (Pin1 > LEFT_MID_POINT_AD && Pin1 <= LEFT_POINT_AD) {
    //        flexStates |= 0b00010000;
    //    } else if (Pin1 > LEFT_POINT_AD && Pin1 <= LEFT_MID_AD) {
    //        flexStates |= 0b00100000;
    //    }
    //
    //    if (Pin2 <= RIGHT_POINT_MID_AD) {
    //        flexStates |= 0b00001100;
    //    } else if ((Pin2 < RIGHT_POINT_MID_AD) && (Pin2 > RIGHT_MID_AD)) {
    //        flexStates |= 0b00001000;
    //    } else if ((Pin2 > RIGHT_POINT_AD) && (Pin2 < RIGHT_MID_AD + 30)) {
    //        flexStates |= 0b00000100;
    //    }
    //
    //    if (Pin3 <= RIGHT_PINKY_RING_AD) {
    //        flexStates |= 0b00000011;
    //    } else if (Pin3 > RIGHT_PINKY_RING_AD && Pin3 <= RIGHT_RING_AD) {
    //        flexStates |= 0b00000010;
    //    } else if (Pin3 > RIGHT_RING_AD && Pin3 <= RIGHT_PINKY_AD) {
    //        flexStates |= 0b00000001;
    //    }
    
    return flexStates;
}

// #define FLEX_CALIBRATION
#define RIGHT_POINT_MIDDLE
#ifdef FLEX_CALIBRATION

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

int main() {
    BOARD_Init();
    FlexInit();
    OledInit();
    char output[100];
    uint8_t flexStates = 0b000000;
    uint8_t flexState = FlexState(flexStates);

#ifdef LEFT_PINKY_RING
    sprintf(output, "Calibrating\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n Left Pinky & Ring");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);

    while (1) {
        flexState = FlexState(flexState);
        OledClear(OLED_COLOR_BLACK);
        sprintf(output, ""BYTE_TO_BINARY_PATTERN"\n\nPin 1 AD:\n\t%d          ", BYTE_TO_BINARY(flexState), AD_ReadADPin(AD_A1));
        printf("%d\n", AD_ReadADPin(AD_A0));
        OledDrawString(output);
        OledUpdate();
    }
#endif
#ifdef LEFT_MIDDLE_POINT
    sprintf(output, "Calibrating\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n Left Middle & Point");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);

    while (1) {
        flexState = FlexState(flexState);
        OledClear(OLED_COLOR_BLACK);
        sprintf(output, ""BYTE_TO_BINARY_PATTERN"\n\nPin 2 AD:\n\t%d          ", BYTE_TO_BINARY(flexState), AD_ReadADPin(AD_A2));
        OledDrawString(output);
        OledUpdate();
    }
#endif
#ifdef RIGHT_POINT_MIDDLE
    sprintf(output, "Calibrating\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n Right Point & Middle");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);

    while (1) {
        flexState = FlexState(flexState);
        OledClear(OLED_COLOR_BLACK);
        sprintf(output, ""BYTE_TO_BINARY_PATTERN"\n\nPin 3 AD:\n\t%d          ", BYTE_TO_BINARY(flexState), AD_ReadADPin(AD_A3));
        OledDrawString(output);
        OledUpdate();
    }
#endif
#ifdef RIGHT_RING_PINKY
    sprintf(output, "Calibrating\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);
    sprintf(output, "Calibrating . . .\n Right Ring & Pinky");
    OledDrawString(output);
    OledUpdate();
    DELAY(A_LOT);

    while (1) {
        flexState = FlexState(flexState);
        OledClear(OLED_COLOR_BLACK);
        sprintf(output, ""BYTE_TO_BINARY_PATTERN"\n\nPin 0 AD:\n\t%d          ", BYTE_TO_BINARY(flexState), AD_ReadADPin(AD_A0));
        OledDrawString(output);
        OledUpdate();
    }
#endif
    BOARD_End();
}
#endif

//This code is perfect will just copy it and put it in the state machine
/**
int main(int argc, char** argv) {
    unsigned int Pin0 = AD_ReadADPin(AD_A0);
    unsigned int Pin1 = AD_ReadADPin(AD_A1);
    unsigned int Pin2 = AD_ReadADPin(AD_A2);
    unsigned int Pin3 = AD_ReadADPin(AD_A3);
    char value[50];

    while (1) {
        if (abs(AD_ReadADPin(AD_A0) - Pin0) > 5) {
            Pin0 = AD_ReadADPin(AD_A0);
           sprintf(value, "Flex Sensor is %d      ", Pin0);
            if(Pin0 <= left_pinkyring){
                sprintf(value, "LeftPinkyRing         ");
                //sprintf(value, "Flex Sensor is %d      ", Pin0);
            }
            else if(Pin0 > left_pinkyring && Pin0 <= left_ring){
               sprintf(value, "LeftRing         ");
            }
            else if(Pin0 > left_ring && Pin0 <= left_pinky){
                sprintf(value, "LeftPinky         ");
            }
            else{
                sprintf(value, "no squeeze        ");
            }
        }
        if (abs(AD_ReadADPin(AD_A1) - Pin1) > 5) {
            Pin1 = AD_ReadADPin(AD_A1);
            //sprintf(value, "Flex Sensor is %d       ", Pin1);
            if(Pin1 <= left_midpoint){
                sprintf(value, "Leftmidpoint          ");
                //sprintf(value, "Flex Sensor is %d      ", Pin0);
            }
            else if(Pin1 > left_midpoint && Pin1 <= left_point){
               sprintf(value, "LeftPoint          ");
            }
            else if(Pin1 > left_point && Pin1 <= left_mid){
                sprintf(value, "LeftMid            ");
            }
            else{
                sprintf(value, "no squeeze         ");                           11000000
            }
        }
        
        if (abs(AD_ReadADPin(AD_A2) - Pin2) > 5) {
            Pin2 = AD_ReadADPin(AD_A2);
            //sprintf(value, "Flex Sensor is %d       ", Pin2);
            if((Pin2 <= right_midpoint)){
                sprintf(value, "Rightmidpoint          ");
                //sprintf(value, "Flex Sensor is %d      ", Pin0);
            }
            else if((Pin2 < right_mid) && (Pin2 > right_midpoint)){
               sprintf(value, "RightPoint          ");
            }
            else if((Pin2 > right_point) && (Pin2 < right_mid + 30)){
                sprintf(value, "RightMid           ");
            }
            else if (Pin2 > right_mid){
                sprintf(value, "no squeeze         ");
            }
        }
        if (abs(AD_ReadADPin(AD_A3) - Pin3) > 5) {
            Pin3 = AD_ReadADPin(AD_A3);
            //sprintf(value, "Flex Sensor is %d      ", Pin3);
            if(Pin3 <= right_pinkyring){
                sprintf(value, "RightPinkyRing         ");
                //sprintf(value, "Flex Sensor is %d      ", Pin0);
            }
            else if(Pin3 > right_pinkyring && Pin3 <= right_ring){
               sprintf(value, "RightRing         ");
            }
            else if(Pin3 > right_ring && Pin3 <= right_pinky){
                sprintf(value, "RightPinky         ");
            }
            else{
                sprintf(value, "no squeeze        ");
            }
        }
        
        //ToneGeneration_SetFrequency(Pin);
        //ToneGeneration_ToneOn();


        OledDrawString(value);
        OledUpdate();
        DELAY(DELAY_GOOD);
    }
    BOARD_End();
}
 */

