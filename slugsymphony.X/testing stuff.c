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
#include <ToneGeneration.h>
#include "flex.h"
#include "9DOF.h"



//#define buttons_stuff
//#define instrument_select
#define playing_guitar

#define DELAY(x)    {int wait; for (wait = 0; wait <= x; wait++) {asm("nop");}} 

typedef enum {
    init,
    guitar,
    drums,
    trumpet,
    piano,
    sax
} instruments;

static instruments curr;

void return_to_init(uint8_t button) {
    if (button != BUTTON_EVENT_NONE) {
        if ((button & BUTTON_EVENT_1DOWN) == 0x02) {
            //LEDS_SET(LEDS_GET() ^ 0x03);
            curr = init;
            button = BUTTON_EVENT_NONE;
        }
    }
}

/*
 * 
 */
int main(int argc, char** argv) {
    BOARD_Init();
    ButtonsInit();
    LEDS_INIT();
    FLEX_INIT();
    ToneGeneration_Init();
    uint8_t curr_butt; //current states of the buttons
    curr = init; //state variable for instrument selection
    int g = 9.81;

    // flags to use for different instrument
    //accelerometers
    int r_acc_x = BNO055_A_ReadAccelX(); //might need to specifiy this later to get two working
    int r_acc_y = BNO055_A_ReadAccelY();
    int r_acc_z = BNO055_A_ReadAccelZ();
    int l_acc_x;
    int l_acc_y; //these need to have the second port working for them
    int l_acc_z;
    
    int movement;
    
    int flex_sensors;

#ifdef instrument_select
    while (1) {
        curr_butt = ButtonsCheckEvents();


        switch (curr) {
            case init:
                printf("current state = Init\n");
                if (flex_sensors) {
                    if (r_acc_z == g) {
                        if (l_acc_z == g) {
                            curr = piano;
                        }
                    }
                }
                if (flex_sensors) {
                    if (r_acc_y == -g) {
                        if (l_acc_y == g) {
                            curr = sax;
                        }
                    }
                }
                if (flex_sensors) {
                    if (r_acc_z == -g) {
                        if (l_acc_z == -g) {
                            curr = drums;
                        }
                    }
                }
                if (flex_sensors) {
                    if (r_acc_x == g) {
                        if (l_acc_x == g) {
                            curr = trumpet;
                        }
                    }
                }
                if (flex_sensors) {
                    if (r_acc_y == g) {
                        if (l_acc_x == g) {
                            curr = guitar;
                        }
                    }
                }



                break;
            case guitar:
                curr_butt = ButtonsCheckEvents();
                printf("current state = guitar\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }

                //code for making noise

                break;
            case sax:
                curr_butt = ButtonsCheckEvents();
                printf("current state = sax\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }

                //code for making noise

                break;
            case drums:
                curr_butt = ButtonsCheckEvents();
                printf("current state = drums\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }

                //code for making noise

                break;
            case trumpet:
                curr_butt = ButtonsCheckEvents();
                printf("current state = trumpet\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }

                //code for making noise

                break;
            case piano:
                curr_butt = ButtonsCheckEvents();
                printf("current state = piano\n");
                if (curr_butt != BUTTON_EVENT_NONE) {
                    return_to_init(curr_butt);
                }

                //code for making noise

                break;
        }
        DELAY(1000);

    }
#endif

#ifdef playing_guitar
    curr_butt = ButtonsCheckEvents();
    printf("current state = guitar\n");
    if (curr_butt != BUTTON_EVENT_NONE) {
        return_to_init(curr_butt);
    }

    if (r_acc_x == 1 || r_acc_y == 1 || r_acc_z == 1) {
        ToneGeneration_ToneOn();
        //If(left pinky){
        //  set tone (lowest note)
        //}
        //if(left ring){
        //  set tone (lower note)
        //}
        //if(left pinky ring){
        //  set tone (low note)
        //}
        //if(left middle){
        //  set tone (high note)
        //}
        //if(left midpoint){
        //  set tone (higher note)
        //}
        //if(left point){
        //  set tone (highest note)
        //}
    } else {
        ToneGeneration_ToneOff();
    }
#endif


#ifdef buttons_stuff
    if (curr_butt != BUTTON_EVENT_NONE) {
        if ((curr_butt & BUTTON_EVENT_2DOWN) == 0x08) {
            LEDS_SET(LEDS_GET() ^ 0x0C);
            curr = guitar;
            curr_butt = BUTTON_EVENT_NONE;
        }
    }


    while (1) {
        curr_butt = ButtonsCheckEvents();
        if (curr_butt != BUTTON_EVENT_NONE) {
            //pressed
            if ((curr_butt & BUTTON_EVENT_1DOWN) == 0x02) {
                LEDS_SET(LEDS_GET() ^ 0x03);
            }
            if ((curr_butt & BUTTON_EVENT_2DOWN) == 0x08) {
                LEDS_SET(LEDS_GET() ^ 0x0C);
            }
            if ((curr_butt & BUTTON_EVENT_3DOWN) == 0x20) {
                LEDS_SET(LEDS_GET() ^ 0x30);
            }
            if ((curr_butt & BUTTON_EVENT_4DOWN) == 0x80) {
                LEDS_SET(LEDS_GET() ^ 0xC0);
            }
            //unpressed
            if ((curr_butt & BUTTON_EVENT_1UP) == 0x01) {
                LEDS_SET(LEDS_GET() ^ 0x03);
            }
            if ((curr_butt & BUTTON_EVENT_2UP) == 0x04) {
                LEDS_SET(LEDS_GET() ^ 0x0C);
            }
            if ((curr_butt & BUTTON_EVENT_3UP) == 0x10) {
                LEDS_SET(LEDS_GET() ^ 0x30);
            }
            if ((curr_butt & BUTTON_EVENT_4UP) == 0x40) {
                LEDS_SET(LEDS_GET() ^ 0xC0);
            }
            curr_butt = BUTTON_EVENT_NONE;
        }
    }
#endif



    return (EXIT_SUCCESS);
}

