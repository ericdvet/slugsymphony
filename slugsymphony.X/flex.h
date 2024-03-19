#ifndef FLEX_H
#define FLEX_H

#include "AD.h"
#include "BOARD.h"

enum right_hand {
    RIGHT_FINGER_ONE = 400,
    RIGHT_FINGER_TWO = 400,
    RIGHT_FINGER_ONE_TWO = 200,
    RIGHT_FINGER_THREE = 992
};

enum left_hand {
    LEFT_FINGER_ONE = 400,
    LEFT_FINGER_TWO_THREE = 600,
    LEFT_FINGER_THREE = 750
};

enum status {
    BENT,
    NOT
};

enum FlexFingerBits {
    RIGHT_BIT_ONE = 0b000100,
    RIGHT_BIT_TWO = 0b000010,
    RIGHT_BIT_THREE = 0b000001,
    LEFT_BIT_ONE = 0b100000,
    LEFT_BIT_TWO = 0b010000,
    LEFT_BIT_THREE = 0b001000,
};

static int L_Pinky;
static int L_Ring;
static int L_PinkyRing;
static int L_Mid;
static int L_Point;
static int L_MidPoint;
static int R_Pinky;
static int R_Ring;
static int R_PinkyRing;
static int R_Mid;
static int R_Point;
static int R_MidPoint;

void FlexInit(void);
//initialiyze the flex sensors

int AllFlexCheck(void);
//checks if all the flex sensors are down or not

uint8_t FlexState(uint8_t flexStates);



#endif 