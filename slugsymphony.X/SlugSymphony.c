/* 
 * File:   statemachine.c
 * Author: Werec
 *
 * Created on March 3, 2024, 5:28 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include "BOARD.h"
#include <Buttons.h>
#include <Leds.h>
#include <xc.h>
#include <stdio.h>
#include <sys/attribs.h>
#include <timers.h>
#include <BNO055.h>
#include "flex.h"
#include "9DOF.h"
#include <ToneGeneration.h>
#include "timers.h"
#include "Oled.h"

//#define instrument_select


#define DELAY(x)    {int wait; for (wait = 0; wait <= x; wait++) {asm("nop");}} 

typedef enum {
    INIT,
    CONFIRMING,
    GUITAR,
    DRUMS,
    TRUMPET,
    PIANO,
    SAX
} InstrumentState;

static InstrumentState instrumentState;

typedef enum {
    RESET_WAITING,
    RESET_INITIATED
} ResetState;

static ResetState resetState;

typedef enum {
    KEY_PRESSED,
    KEY_DELAY,
    KEY_RELEASED
} PianoStates;

static PianoStates leftKey1State;
static PianoStates leftKey2State;
static PianoStates leftKey3State;
static PianoStates rightKey1State;
static PianoStates rightKey2State;
static PianoStates rightKey3State;

static PianoStates leftTrumpet1State;
static PianoStates leftTrumpet2State;
static PianoStates leftTrumpet3State;
static PianoStates rightTrumpet1State;
static PianoStates rightTrumpet2State;
static PianoStates rightTrumpet3State;

typedef enum {
    NOT_SAX,
    PLAYING_NEW_TONE,
    PLAYING_ACTIVE
} SaxStates;

static SaxStates saxStates;
static SaxStates trumpetStates;

typedef enum {
    L_BIN_1,
    L_BIN_2,
    L_BIN_3,
    R_BIN_1,
    R_BIN_2,
    R_BIN_3
} DrumBins;

static DrumBins drumLeftBins;
static DrumBins drumRightBins;
static float drumGyroLeftCount;
static float drumGyroRightCount;

#define DRUM_MOVEMENT 45

typedef enum {
    DRUM_RESET,
    DRUM_WAIT,
    DRUM_DELAY,
    DRUM_HIT
} DrumStates;

static DrumStates drumLeftStates;
static DrumStates drumRightStates;
static DrumStates guitarStates;

typedef enum {
    MOVING,
    NOT_MOVING
} InstrumentMovement;

static InstrumentMovement instrumentMovement;
static int instrumentMovementBuffer;
#define MOVEMENT_BUFFER_NUM 50

//gets the flags from the 9dof

/**
extern int LFLAGZ;
extern int LFLAGY;
extern int LFLAGX;
extern int RFLAGZ;
extern int RFLAGY;
extern int RFLAGX;
 */

void return_to_init(uint8_t button) {
    if (button != BUTTON_EVENT_NONE) {
        if ((button & BUTTON_EVENT_1DOWN) == 0x02) {
            //LEDS_SET(LEDS_GET() ^ 0x03);
            instrumentState = INIT;
            button = BUTTON_EVENT_NONE;
        }
    }
}

#define TEMP_STATE_MACHINE // without flex sensors
#ifdef TEMP_STATE_MACHINE

int main() {
    BOARD_Init();

    printf("TEMP STATE MACHINE v2 (State Machine Test)\n\n");

    DOF_INIT();
    FlexInit();
    TIMERS_Init();

    printf("BNO055 Configured!\n\n");

    char instrumentLeftNote[100];
    char instrumentRightNote[100];

    uint8_t flexySelexy = 0b000000;
    uint8_t flexReading = FlexState(flexySelexy);
    uint8_t prevFlexReading = 0b00000000;
    uint8_t newFlexInputs;

    instrumentState = INIT;
    InstrumentState instrumentConfirmation = INIT;
    int currentTime = TIMERS_GetMilliSeconds();

    instrumentMovement = NOT_MOVING;
    instrumentMovementBuffer = 0;
    resetState = RESET_WAITING;

    // piano

    leftKey1State = KEY_RELEASED;
    leftKey2State = KEY_RELEASED;
    leftKey3State = KEY_RELEASED;
    rightKey1State = KEY_RELEASED;
    rightKey2State = KEY_RELEASED;
    rightKey3State = KEY_RELEASED;

    leftTrumpet1State = KEY_RELEASED;
    leftTrumpet2State = KEY_RELEASED;
    leftTrumpet3State = KEY_RELEASED;
    rightTrumpet1State = KEY_RELEASED;
    rightTrumpet2State = KEY_RELEASED;
    rightTrumpet3State = KEY_RELEASED;

    float keyDelayL1, keyDelayL2, keyDelayL3, keyDelayR1, keyDelayR2, keyDelayR3;
    keyDelayL1 = 0;
    keyDelayL2 = 0;
    keyDelayL3 = 0;
    keyDelayR1 = 0;
    keyDelayR2 = 0;
    keyDelayR3 = 0;

    float trumpetDelayL1, trumpetDelayL2, trumpetDelayL3, trumpetDelayR1, trumpetDelayR2, trumpetDelayR3;
    trumpetDelayL1 = 0;
    trumpetDelayL2 = 0;
    trumpetDelayL3 = 0;
    trumpetDelayR1 = 0;
    trumpetDelayR2 = 0;
    trumpetDelayR3 = 0;

    int drumDelayL = 0;
    int drumDelayR = 0;

    // Sax stuff
    saxStates = NOT_SAX;

    // Trumpet stuff
    trumpetStates = NOT_SAX;

    // Drum stuff
    drumLeftBins = L_BIN_3;
    drumRightBins = R_BIN_1;
    drumGyroLeftCount = 0;
    drumGyroRightCount = 0;
    drumLeftStates = DRUM_RESET;
    drumRightStates = DRUM_RESET;

    // guitar
    guitarStates = DRUM_HIT;


    while (1) {
        char LX = CheckIMUStateLX();
        char LY = CheckIMUStateLY();
        char LZ = CheckIMUStateLZ();
        char RX = CheckIMUStateRX();
        char RY = CheckIMUStateRY();
        char RZ = CheckIMUStateRZ();

        float current_A_GyroX = (0.00158355565428341 * (BNO055_A_ReadGyroX() - 2.94000000000000));
        float current_A_GyroY = (0.00148691821743288 * (BNO055_A_ReadGyroY() - 13.0500000000000));
        float current_A_GyroZ = (0.00148757871770715 * (BNO055_A_ReadGyroZ() + 14.9733333333333));

        float current_B_GyroX = (0.00158355565428341 * (BNO055_B_ReadGyroX() - 2.94000000000000));
        float current_B_GyroY = (0.00148691821743288 * (BNO055_B_ReadGyroY() - 13.0500000000000));
        float current_B_GyroZ = (0.00148757871770715 * (BNO055_B_ReadGyroZ() + 14.9733333333333));

        flexReading = FlexState(flexReading);
        newFlexInputs = flexReading & ~prevFlexReading;

        switch (resetState) {
            case RESET_WAITING:
                if (LZ == NEGATIVE && RZ == NEGATIVE) {
                    currentTime = TIMERS_GetMilliSeconds();
                    resetState = RESET_INITIATED;
                }
                break;

            case RESET_INITIATED:
                if (abs(currentTime - TIMERS_GetMilliSeconds()) > 5000) {
                    resetState = RESET_WAITING;
                    printf("Tacet\n");
                    printf("buffer\n");
                    printf("startup_tone.mp3\n");
                    printf("buffer\n");
                    instrumentState = INIT;
                }
                break;

            default:
                break;
        }

        switch (instrumentMovement) {
            case NOT_MOVING:
                if (InstrumentMoved()) {
                    instrumentMovementBuffer++;
                }
                if (instrumentMovementBuffer > MOVEMENT_BUFFER_NUM) {
                    instrumentMovementBuffer = 0;
                    instrumentMovement = MOVING;
                }
                break;

            case MOVING:
                if (!InstrumentMoved()) {
                    instrumentMovementBuffer++;
                }
                if (instrumentMovementBuffer > MOVEMENT_BUFFER_NUM) {
                    instrumentMovementBuffer = 0;
                    instrumentMovement = NOT_MOVING;
                }
                break;

            default:
                break;
        }

        switch (instrumentState) {
            case INIT:
                if (RZ == POSITIVE && LZ == POSITIVE) { // Piano
                    instrumentConfirmation = PIANO;
                    instrumentState = CONFIRMING;
                    currentTime = TIMERS_GetMilliSeconds();
                    printf("d4.mp3\n");
                    printf("buffer\n");
                    break;
                }
                if (RY == POSITIVE && LY == NEGATIVE) { // drums & sax
                    if (flexReading & 0b001100) { // drums if flex pressed
                        instrumentConfirmation = DRUMS;
                        instrumentState = CONFIRMING;
                        currentTime = TIMERS_GetMilliSeconds();
                        printf("d4.mp3\n");
                        printf("buffer\n");
                        break;
                    } else { // sax
                        instrumentConfirmation = SAX;
                        instrumentState = CONFIRMING;
                        currentTime = TIMERS_GetMilliSeconds();
                        printf("d4.mp3\n");
                        printf("buffer\n");
                        break;
                    }
                }
                if (RX == POSITIVE && LX == POSITIVE) { // Trumpet
                    instrumentConfirmation = TRUMPET;
                    instrumentState = CONFIRMING;
                    currentTime = TIMERS_GetMilliSeconds();
                    printf("d4.mp3\n");
                    printf("buffer\n");
                    break;
                }
                if (LX == POSITIVE && RY == POSITIVE) { // Guitar
                    instrumentConfirmation = GUITAR;
                    instrumentState = CONFIRMING;
                    currentTime = TIMERS_GetMilliSeconds();
                    printf("d4.mp3\n");
                    printf("buffer\n");
                    break;
                }
                break;

            case CONFIRMING:
                if (abs(TIMERS_GetMilliSeconds() - currentTime) >= 3000) {
                    if (instrumentConfirmation == PIANO) {
                        if (RZ == POSITIVE && LZ == POSITIVE) { // Piano
                            instrumentState = PIANO;
                            printf("startup_tone.mp3\n");
                            printf("buffer\n");
                            break;
                        } else {
                            instrumentState = INIT;
                            printf("Instrument lost\n");
                            break;
                        }
                    } else if (instrumentConfirmation == SAX) {
                        if (RY == POSITIVE && LY == NEGATIVE) { // Sax
                            instrumentState = SAX;
                            printf("startup_tone.mp3\n");
                            printf("buffer\n");
                            break;
                        } else {
                            instrumentState = INIT;
                            printf("Instrument lost\n");
                            break;
                        }
                    } else if (instrumentConfirmation == DRUMS) {
                        if (RY == POSITIVE && LY == NEGATIVE) { // Drums
                            instrumentState = DRUMS;
                            printf("startup_tone.mp3\n");
                            printf("buffer\n");
                            break;
                        } else {
                            instrumentState = INIT;
                            printf("Instrument lost\n");
                            break;
                        }
                    } else if (instrumentConfirmation == TRUMPET) {
                        if (RX == POSITIVE && LX == POSITIVE) { // Trumpet
                            instrumentState = TRUMPET;
                            printf("startup_tone.mp3\n");
                            printf("buffer\n");
                            break;
                        } else {
                            instrumentState = INIT;
                            printf("Instrument lost\n");
                            break;
                        }
                    } else if (instrumentConfirmation == GUITAR) {
                        if (LX == POSITIVE && RY == POSITIVE) { // Guitar
                            instrumentState = GUITAR;
                            printf("startup_tone.mp3\n");
                            printf("buffer\n");
                            break;
                        } else {
                            instrumentState = INIT;
                            printf("Instrument lost\n");
                            break;
                        }
                    }
                }
                break;

            case PIANO:

                switch (leftKey1State) {
                    case KEY_RELEASED:
                        if (flexReading & LEFT_BIT_ONE) {
                            printf("b3 (1).mp3\n");
                            printf("buffer\n");
                            leftKey1State = KEY_DELAY;
                            keyDelayL1 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(keyDelayL1 - TIMERS_GetMilliSeconds()) > 200) {
                            leftKey1State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & LEFT_BIT_ONE)) {
                            leftKey1State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (leftKey2State) {
                    case KEY_RELEASED:
                        if (flexReading & LEFT_BIT_TWO) {
                            printf("Tacet\n");
                            printf("Buffer\n");
                            printf("c3 (1).mp3\n");
                            printf("buffer\n");
                            leftKey2State = KEY_PRESSED;
                            keyDelayL2 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(keyDelayL2 - TIMERS_GetMilliSeconds()) > 200) {
                            leftKey2State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & LEFT_BIT_TWO)) {
                            leftKey2State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (leftKey3State) {
                    case KEY_RELEASED:
                        if (flexReading & LEFT_BIT_THREE) {
                            if (!(flexReading & 0b010000)) {
                                printf("d3 (1).mp3\n");
                                printf("buffer\n");
                                leftKey3State = KEY_PRESSED;
                                keyDelayL3 = TIMERS_GetMilliSeconds();
                            }
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(keyDelayL3 - TIMERS_GetMilliSeconds()) > 200) {
                            leftKey3State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & LEFT_BIT_THREE)) {
                            leftKey3State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (rightKey1State) {
                    case KEY_RELEASED:
                        if (flexReading & RIGHT_BIT_ONE) {
                            if (!(flexReading & 0b000010)) {
                                printf("e3 (1).mp3\n");
                                printf("buffer\n");
                                rightKey1State = KEY_PRESSED;
                                keyDelayR1 = TIMERS_GetMilliSeconds();
                            }
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(keyDelayR1 - TIMERS_GetMilliSeconds()) > 200) {
                            rightKey1State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & RIGHT_BIT_ONE)) {
                            rightKey1State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (rightKey2State) {
                    case KEY_RELEASED:
                        if (flexReading & RIGHT_BIT_TWO) {
                            printf("Tacet\n");
                            printf("Buffer\n");
                            printf("f3 (1).mp3\n");
                            printf("buffer\n");
                            rightKey2State = KEY_PRESSED;
                            keyDelayR2 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(keyDelayR2 - TIMERS_GetMilliSeconds()) > 200) {
                            rightKey2State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & RIGHT_BIT_TWO)) {
                            rightKey2State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (rightKey3State) {
                    case KEY_RELEASED:
                        if (flexReading & RIGHT_BIT_THREE) {
                            printf("g3 (2).mp3\n");
                            printf("buffer\n");
                            rightKey3State = KEY_PRESSED;
                            keyDelayR3 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(keyDelayR3 - TIMERS_GetMilliSeconds()) > 200) {
                            rightKey3State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & RIGHT_BIT_THREE)) {
                            rightKey3State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                break;

            case SAX:

                if (flexReading & 0b100000) {
                    sprintf(instrumentRightNote, "saxa 1.wav");
                } else if (flexReading & 0b010000) {
                    sprintf(instrumentRightNote, "saxa 2.wav");
                } else if (flexReading & 0b001000) {
                    sprintf(instrumentRightNote, "saxa 3.wav");
                } else if (flexReading & 0b000010) {
                    sprintf(instrumentRightNote, "saxa 5.wav");
                } else if (flexReading & 0b000100) {
                    sprintf(instrumentRightNote, "saxa 4.wav");
                } else {
                    sprintf(instrumentRightNote, "saxa 6.wav");
                }

                switch (saxStates) {
                    case NOT_SAX:
                        if (InstrumentMoved()) {
                            saxStates = PLAYING_NEW_TONE;
                        }
                        break;

                    case PLAYING_NEW_TONE:
                        printf("%s\n", instrumentRightNote);
                        printf("buffer\n");
                        currentTime = TIMERS_GetMilliSeconds();
                        saxStates = PLAYING_ACTIVE;
                        break;

                    case PLAYING_ACTIVE:
                        if (abs(currentTime - TIMERS_GetMilliSeconds()) > 5000) {
                            if (!InstrumentMoved()) {
                                saxStates = NOT_SAX;
                                break;
                            }
                            saxStates = PLAYING_NEW_TONE;
                            printf("Tacet\n");
                            printf("buffer\n");
                        }
                        break;

                    default:
                        break;
                }

                break;

            case DRUMS:

                drumGyroLeftCount += current_A_GyroY;
                drumGyroRightCount -= current_B_GyroY;

                switch (drumLeftBins) {
                    case L_BIN_1:
                        sprintf(instrumentLeftNote, "CYCdh_Crash-01.wav");
                        if (drumGyroLeftCount < -DRUM_MOVEMENT) {
                            drumGyroLeftCount = 0;
                        }
                        if (drumGyroLeftCount > DRUM_MOVEMENT) {
                            drumLeftBins = L_BIN_2;
                            drumGyroLeftCount = 0;
                        }
                        break;

                    case L_BIN_2:
                        sprintf(instrumentLeftNote, "CYCdh_Crash-02.wav");
                        if (drumGyroLeftCount < -DRUM_MOVEMENT) {
                            drumLeftBins = L_BIN_1;
                            drumGyroLeftCount = 0;
                        }
                        if (drumGyroLeftCount > DRUM_MOVEMENT) {
                            drumLeftBins = L_BIN_3;
                            drumGyroLeftCount = 0;
                        }
                        break;

                    case L_BIN_3:
                        sprintf(instrumentLeftNote, "CYCdh_Crash-03.wav");
                        if (drumGyroLeftCount < -DRUM_MOVEMENT) {
                            drumLeftBins = L_BIN_2;
                            drumGyroLeftCount = 0;
                        }
                        if (drumGyroLeftCount > DRUM_MOVEMENT) {
                            drumGyroLeftCount = 0;
                        }
                        break;

                    default:
                        break;
                }

                switch (drumRightBins) {
                    case R_BIN_1:
                        sprintf(instrumentRightNote, "Acoustic Snare-01.wav");
                        if (drumGyroRightCount < -DRUM_MOVEMENT) {
                            drumGyroRightCount = 0;
                        }
                        if (drumGyroRightCount > DRUM_MOVEMENT) {
                            drumRightBins = R_BIN_2;
                            drumGyroRightCount = 0;
                        }
                        break;

                    case R_BIN_2:
                        sprintf(instrumentRightNote, "Acoustic Snare-02.wav");
                        if (drumGyroRightCount < -DRUM_MOVEMENT) {
                            drumRightBins = R_BIN_1;
                            drumGyroRightCount = 0;
                        }
                        if (drumGyroRightCount > DRUM_MOVEMENT) {
                            drumRightBins = R_BIN_3;
                            drumGyroRightCount = 0;
                        }
                        break;

                    case R_BIN_3:
                        sprintf(instrumentRightNote, "Acoustic Snare-03.wav");
                        if (drumGyroRightCount < -DRUM_MOVEMENT) {
                            drumRightBins = R_BIN_2;
                            drumGyroRightCount = 0;
                        }
                        if (drumGyroRightCount > DRUM_MOVEMENT) {
                            drumGyroRightCount = 0;
                        }
                        break;

                    default:
                        break;
                }

                //                printf("test");

                switch (drumLeftStates) {
                    case DRUM_RESET:
                        //                        printf("test\n");
                        if (DrumHit() & 0b10) {
                            drumLeftStates = DRUM_WAIT;

                        }
                        break;

                    case DRUM_WAIT:
                        if (LY != NEGATIVE) {
                            drumLeftStates = DRUM_DELAY;
                            printf("%s\n", instrumentLeftNote);
                            printf("buffer\n");
                            drumDelayL = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case DRUM_DELAY:
                        if (abs(drumDelayL - TIMERS_GetMilliSeconds()) > 200) {
                            drumLeftStates = DRUM_HIT;
                        }
                        break;

                    case DRUM_HIT:
                        if (LY == NEGATIVE) {
                            drumLeftStates = DRUM_RESET;
                        }
                        break;

                    default:
                        break;
                }

                switch (drumRightStates) {
                    case DRUM_RESET:
                        if (DrumHit() & 0b01) {
                            drumRightStates = DRUM_WAIT;

                        }
                        break;

                    case DRUM_WAIT:
                        if (RY != POSITIVE) {
                            drumRightStates = DRUM_DELAY;
                            printf("%s\n", instrumentRightNote);
                            printf("buffer\n");
                            drumDelayR = TIMERS_GetMilliSeconds();

                        }
                        break;

                    case DRUM_DELAY:
                        if (abs(drumDelayR - TIMERS_GetMilliSeconds()) > 200) {
                            drumRightStates = DRUM_HIT;
                        }
                        break;

                    case DRUM_HIT:
                        if (RY == POSITIVE) {
                            drumRightStates = DRUM_RESET;
                        }
                        break;

                    default:
                        break;
                }

                //                printf("%d - %d\n", drumLeftBins, drumRightBins);

                break;

            case TRUMPET:

                switch (leftTrumpet1State) {
                    case KEY_RELEASED:
                        if (flexReading & LEFT_BIT_ONE) {
                            printf("trumpet-C4.wav\n");
                            printf("buffer\n");
                            leftTrumpet1State = KEY_DELAY;
                            trumpetDelayL1 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(trumpetDelayL1 - TIMERS_GetMilliSeconds()) > 200) {
                            leftTrumpet1State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & LEFT_BIT_ONE)) {
                            leftTrumpet1State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (leftTrumpet2State) {
                    case KEY_RELEASED:
                        if (flexReading & LEFT_BIT_TWO) {
                            printf("Tacet\n");
                            printf("Buffer\n");
                            printf("trumpet-C5.wav\n");
                            printf("buffer\n");
                            leftTrumpet2State = KEY_PRESSED;
                            trumpetDelayL2 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(trumpetDelayL2 - TIMERS_GetMilliSeconds()) > 200) {
                            leftTrumpet2State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & LEFT_BIT_TWO)) {
                            leftTrumpet2State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (leftTrumpet3State) {
                    case KEY_RELEASED:
                        if (flexReading & LEFT_BIT_THREE) {
                            if (!(flexReading & 0b010000)) {
                                printf("trumpet-G3.wav\n");
                                printf("buffer\n");
                                leftTrumpet3State = KEY_PRESSED;
                                trumpetDelayL3 = TIMERS_GetMilliSeconds();
                            }
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(trumpetDelayL3 - TIMERS_GetMilliSeconds()) > 200) {
                            leftTrumpet3State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & LEFT_BIT_THREE)) {
                            leftTrumpet3State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (rightTrumpet1State) {
                    case KEY_RELEASED:
                        if (flexReading & RIGHT_BIT_ONE) {
                            if (!(flexReading & 0b000010)) {
                                printf("trumpet-G4.wav\n");
                                printf("buffer\n");
                                rightTrumpet1State = KEY_PRESSED;
                                trumpetDelayR1 = TIMERS_GetMilliSeconds();
                            }
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(trumpetDelayR1 - TIMERS_GetMilliSeconds()) > 200) {
                            rightTrumpet1State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & RIGHT_BIT_ONE)) {
                            rightTrumpet1State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (rightTrumpet2State) {
                    case KEY_RELEASED:
                        if (flexReading & RIGHT_BIT_TWO) {
                            printf("Tacet\n");
                            printf("Buffer\n");
                            printf("trumpet-G5.wav\n");
                            printf("buffer\n");
                            rightTrumpet2State = KEY_PRESSED;
                            trumpetDelayR2 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(trumpetDelayR2 - TIMERS_GetMilliSeconds()) > 200) {
                            rightTrumpet2State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & RIGHT_BIT_TWO)) {
                            rightTrumpet2State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                switch (rightTrumpet3State) {
                    case KEY_RELEASED:
                        if (flexReading & RIGHT_BIT_THREE) {
                            printf("trumpet-wat.wav\n");
                            printf("buffer\n");
                            rightTrumpet3State = KEY_PRESSED;
                            trumpetDelayR3 = TIMERS_GetMilliSeconds();
                        }
                        break;

                    case KEY_DELAY:
                        if (abs(trumpetDelayR3 - TIMERS_GetMilliSeconds()) > 200) {
                            rightTrumpet3State = KEY_PRESSED;
                        }
                        break;

                    case KEY_PRESSED:
                        if (!(flexReading & RIGHT_BIT_THREE)) {
                            rightTrumpet3State = KEY_RELEASED;
                        }
                        break;

                    default:
                        break;
                }

                break;

            case GUITAR:


                //                printf("%d\n", RY);
                if (flexReading & 0b100000) {
                    sprintf(instrumentRightNote, "guitar C_64kb.mp3");
                } else if (flexReading & 0b010000) {
                    sprintf(instrumentRightNote, "guitar D_64kb.mp3");
                } else if (flexReading & 0b001000) {
                    sprintf(instrumentRightNote, "guitar Dm_64kb.mp3");
                } else {
                    sprintf(instrumentRightNote, "guitar E_64kb.mp3");
                }

                //                switch (guitarStates) {
                //                    case DRUM_RESET:
                //                        //                        printf("test\n");
                //                        if (RY == POSITIVE) {
                //                            guitarStates = DRUM_WAIT;
                //                            currentTime = TIMERS_GetMilliSeconds();
                //
                //                        }
                //                        break;
                //
                //                    case DRUM_WAIT:
                //                        if (abs(TIMERS_GetMilliSeconds() - currentTime) > 200) {
                //                            guitarStates = DRUM_HIT;
                //                        }
                //                        break;
                //
                //                    case DRUM_HIT:
                //                        if (RY != POSITIVE) {
                //                            guitarStates = DRUM_RESET;
                //                            printf("%s\n", instrumentRightNote);
                //                            printf("buffer\n");
                //                        }
                //                        break;
                //
                //                    default:
                //                        break;
                //                }

                switch (guitarStates) {
                    case DRUM_RESET:
                        if (DrumHit()) {
                            guitarStates = DRUM_WAIT;

                        }
                        break;

                    case DRUM_WAIT:
                        if (RY != POSITIVE) {
                            guitarStates = DRUM_DELAY;
                            printf("%s\n", instrumentRightNote);
                            printf("buffer\n");
                            drumDelayR = TIMERS_GetMilliSeconds();

                        }
                        break;

                    case DRUM_DELAY:
                        if (abs(drumDelayR - TIMERS_GetMilliSeconds()) > 200) {
                            guitarStates = DRUM_HIT;
                        }
                        break;

                    case DRUM_HIT:
                        if (RY == POSITIVE) {
                            guitarStates = DRUM_RESET;
                        }
                        break;

                    default:
                        break;
                }


                break;

            default:
                break;
        }

        prevFlexReading = flexReading;

    }

    BOARD_End();
    return (EXIT_SUCCESS);
}
#endif

// #define STATE_MACHINE
#ifdef STATE_MACHINE

/*
 * 
 */
int main(int argc, char** argv) {
    BOARD_Init();
    ButtonsInit();
    LEDS_INIT();
    FlexInit();

    //state machine shit
    uint8_t curr_butt; //current states of the buttons
    instrumentState = INIT; //state variable for instrument selection
    //int g = 9.81;


    //accelerometers
    int any_acc; //flag for any movement on the accellerometers
    int r_any_acc;

    /*
    int r_acc_x = BNO055_B_ReadAccelX(); //might need to specifiy this later to get two working
    int r_acc_y = BNO055_B_ReadAccelY();
    int r_acc_z = BNO055_B_ReadAccelZ();
    int l_acc_x = BNO055_A_ReadAccelX();
    int l_acc_y = BNO055_A_ReadAccelY(); //these need to have the second port working for them
    int l_acc_z = BNO055_A_ReadAccelZ();
     */

    //flex sensors
    int flex_sensors; //flag for all flex sensors being bent

    unsigned int Pin0 = AD_ReadADPin(AD_A0);
    unsigned int Pin1 = AD_ReadADPin(AD_A1);
    unsigned int Pin2 = AD_ReadADPin(AD_A2);
    unsigned int Pin3 = AD_ReadADPin(AD_A3);

#ifdef instrument_select
    while (1) {
        curr_butt = ButtonsCheckEvents();
        flex_sensors = AllFlexCheck();
        any_acc = AnyAccell();
        r_any_acc = RAnyAccell();
        CheckIMUStateL();
        CheckIMUStateR();

        switch (instrumentState) {
            case INIT:
                printf("current state = Init\n");

                if (flex_sensors) {
                    if (RFLAGZ == POSITIVE) {
                        if (LFLAGZ == POSITIVE) {
                            instrumentState = PIANO;
                        }
                    } else if (RFLAGY == NEGATIVE) {
                        if (LFLAGY == POSITIVE) {
                            instrumentState = SAX;
                        }
                    } else if (RFLAGZ == NEGATIVE) {
                        if (LFLAGZ == NEGATIVE) {
                            instrumentState = DRUMS;
                        }
                    } else if (RFLAGX == POSITIVE) {
                        if (LFLAGX == POSITIVE) {
                            instrumentState = TRUMPET;
                        }
                    } else if (RFLAGY == POSITIVE) {
                        if (LFLAGX == POSITIVE) {
                            instrumentState = GUITAR;
                        }
                    }
                }
                break;

            case GUITAR:
                curr_butt = ButtonsCheckEvents();
                printf("current state = guitar\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }
                //code for making noise
                //might need to add reread the acc
                //need to change the acc to gyro
                if (r_any_acc) {
                    if (abs(AD_ReadADPin(AD_A0) - Pin0) > 5) {
                        Pin0 = AD_ReadADPin(AD_A0);
                        if (Pin0 <= LEFT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_PINKY_RING && Pin0 <= LEFT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_RING && Pin0 <= LEFT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A1) - Pin1) > 5) {
                        Pin1 = AD_ReadADPin(AD_A1);
                        if (Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_MID && Pin1 <= LEFT_POINT) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_POINT && Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }
                }
                break;

            case SAX:
                curr_butt = ButtonsCheckEvents();
                printf("current state = sax\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }
                //code for making noise
                if (any_acc) {
                    //check any accc here
                    //left hand
                    if (abs(AD_ReadADPin(AD_A0) - Pin0) > 5) {
                        Pin0 = AD_ReadADPin(AD_A0);
                        if (Pin0 <= LEFT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_PINKY_RING && Pin0 <= LEFT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_RING && Pin0 <= LEFT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A1) - Pin1) > 5) {
                        Pin1 = AD_ReadADPin(AD_A1);
                        if (Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_MID && Pin1 <= LEFT_POINT) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_POINT && Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                    //right hand
                    if (abs(AD_ReadADPin(AD_A2) - Pin2) > 5) {
                        Pin2 = AD_ReadADPin(AD_A2);
                        if ((Pin2 <= RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 < RIGHT_MID) && (Pin2 > RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 > RIGHT_POINT) && (Pin2 < RIGHT_MID + 30)) {
                            //printf("name of mp3 file");
                            //printf("buffer");  
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A3) - Pin3) > 5) {
                        Pin3 = AD_ReadADPin(AD_A3);
                        if (Pin3 <= RIGHT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_PINKY_RING && Pin3 <= RIGHT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_RING && Pin3 <= RIGHT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                }
                break;

            case DRUMS:
                curr_butt = ButtonsCheckEvents();
                printf("current state = drums\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }
                //code for making noise
                if (any_acc) {
                    //left hand
                    if (abs(AD_ReadADPin(AD_A0) - Pin0) > 5) {
                        Pin0 = AD_ReadADPin(AD_A0);
                        if (Pin0 <= LEFT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_PINKY_RING && Pin0 <= LEFT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_RING && Pin0 <= LEFT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A1) - Pin1) > 5) {
                        Pin1 = AD_ReadADPin(AD_A1);
                        if (Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_MID && Pin1 <= LEFT_POINT) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_POINT && Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                    //right hand
                    if (abs(AD_ReadADPin(AD_A2) - Pin2) > 5) {
                        Pin2 = AD_ReadADPin(AD_A2);
                        if ((Pin2 <= RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 < RIGHT_MID) && (Pin2 > RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 > RIGHT_POINT) && (Pin2 < RIGHT_MID + 30)) {
                            //printf("name of mp3 file");
                            //printf("buffer");  
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A3) - Pin3) > 5) {
                        Pin3 = AD_ReadADPin(AD_A3);
                        if (Pin3 <= RIGHT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_PINKY_RING && Pin3 <= RIGHT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_RING && Pin3 <= RIGHT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                }
                break;

            case TRUMPET:
                curr_butt = ButtonsCheckEvents();
                printf("current state = trumpet\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }
                //code for making noise
                if (any_acc) {
                    //left hand
                    if (abs(AD_ReadADPin(AD_A0) - Pin0) > 5) {
                        Pin0 = AD_ReadADPin(AD_A0);
                        if (Pin0 <= LEFT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_PINKY_RING && Pin0 <= LEFT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_RING && Pin0 <= LEFT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A1) - Pin1) > 5) {
                        Pin1 = AD_ReadADPin(AD_A1);
                        if (Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_MID && Pin1 <= LEFT_POINT) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_POINT && Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                    //right hand
                    if (abs(AD_ReadADPin(AD_A2) - Pin2) > 5) {
                        Pin2 = AD_ReadADPin(AD_A2);
                        if ((Pin2 <= RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 < RIGHT_MID) && (Pin2 > RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 > RIGHT_POINT) && (Pin2 < RIGHT_MID + 30)) {
                            //printf("name of mp3 file");
                            //printf("buffer");  
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A3) - Pin3) > 5) {
                        Pin3 = AD_ReadADPin(AD_A3);
                        if (Pin3 <= RIGHT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_PINKY_RING && Pin3 <= RIGHT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_RING && Pin3 <= RIGHT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                }
                break;

            case PIANO:
                curr_butt = ButtonsCheckEvents();
                printf("current state = piano\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }
                //code for making noise
                if (any_acc) {
                    //left hand
                    if (abs(AD_ReadADPin(AD_A0) - Pin0) > 5) {
                        Pin0 = AD_ReadADPin(AD_A0);
                        if (Pin0 <= LEFT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_PINKY_RING && Pin0 <= LEFT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin0 > LEFT_RING && Pin0 <= LEFT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A1) - Pin1) > 5) {
                        Pin1 = AD_ReadADPin(AD_A1);
                        if (Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_MID && Pin1 <= LEFT_POINT) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin1 > LEFT_POINT && Pin1 <= LEFT_MID) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                    //right hand
                    if (abs(AD_ReadADPin(AD_A2) - Pin2) > 5) {
                        Pin2 = AD_ReadADPin(AD_A2);
                        if ((Pin2 <= RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 < RIGHT_MID) && (Pin2 > RIGHT_MID)) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if ((Pin2 > RIGHT_POINT) && (Pin2 < RIGHT_MID + 30)) {
                            //printf("name of mp3 file");
                            //printf("buffer");  
                        }
                    }
                    if (abs(AD_ReadADPin(AD_A3) - Pin3) > 5) {
                        Pin3 = AD_ReadADPin(AD_A3);
                        if (Pin3 <= RIGHT_PINKY_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_PINKY_RING && Pin3 <= RIGHT_RING) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        } else if (Pin3 > RIGHT_RING && Pin3 <= RIGHT_PINKY) {
                            //printf("name of mp3 file");
                            //printf("buffer");
                        }
                    }

                }
                break;
        }
        DELAY(1000);

    }
#endif
    return (EXIT_SUCCESS);
}
#endif

//#define INSTRUMENT_CONFIG_TEST
#ifdef INSTRUMENT_CONFIG_TEST

int main() {
    BOARD_Init();
    DOF_INIT();

    while (1) {
        char LX = CheckIMUStateLX();
        char LY = CheckIMUStateLY();
        char LZ = CheckIMUStateLZ();
        char RX = CheckIMUStateRX();
        char RY = CheckIMUStateRY();
        char RZ = CheckIMUStateRZ();

        printf("L: (%d, %d, %d) - R: (%d, %d, %d)\n\n", LX, LY, LZ,
                RX, RY, RZ);
        DELAY(183000);

        if (1) {
            if (RZ == POSITIVE && LZ == POSITIVE) {
                instrumentState = PIANO;
                printf("Piano\n");
            }
            if (RY == POSITIVE && LY == NEGATIVE) {
                instrumentState = SAX;
                printf("Sax\n");
            }
            if (RY == POSITIVE && LY == NEGATIVE) { // and flex sensors 
                instrumentState = DRUMS;
                printf("Drums\n");
            }
            if (RX == POSITIVE && LX == POSITIVE) {
                instrumentState = TRUMPET;
                printf("Trumpet\n");
            }
            if (LX == POSITIVE && RY == POSITIVE) {
                instrumentState = GUITAR;
                printf("Guitar\n");
            }
        }
    }

    BOARD_End();
    return (EXIT_SUCCESS);
}
#endif

//#define KEY_TEST
#ifdef KEY_TEST

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
    DOF_INIT();
    FlexInit();
    OledInit();
    char output[100];
    uint8_t flexySelexy = 0b000000;
    uint8_t flexReading = FlexState(flexySelexy);
    uint8_t prevFlexReading = 0b00000000;
    uint8_t newFlexInputs;

    while (1) {
        flexReading = FlexState(flexReading);
        newFlexInputs = flexReading & ~prevFlexReading;

        if (newFlexInputs & LEFT_BIT_ONE) {
            printf("c4.mp3\n");
            printf("buffer\n");
        }

        if (newFlexInputs & LEFT_BIT_TWO) {
            printf("d4.mp3\n");
            printf("buffer\n");
        }

        if (newFlexInputs & LEFT_BIT_THREE) {
            printf("e4.mp3\n");
            printf("buffer\n");
        }

        if (flexReading & RIGHT_BIT_ONE) {
            printf("f4.mp3\n");
            printf("buffer\n");
        }

        if (flexReading & RIGHT_BIT_TWO) {
            printf("g4.mp3\n");
            printf("buffer\n");
        }

        if (newFlexInputs & RIGHT_BIT_THREE) {
            printf("a5.mp3\n");
            printf("buffer\n");
        }


        prevFlexReading = flexReading;

        OledClear(OLED_COLOR_BLACK);
        sprintf(output, ""BYTE_TO_BINARY_PATTERN"\n\nPin 0 AD:\n\t%d          ", BYTE_TO_BINARY(flexReading), AD_ReadADPin(AD_A0));
        OledDrawString(output);
        OledUpdate();
    }


    BOARD_End();
    return (EXIT_SUCCESS);
}
#endif