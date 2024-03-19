/* 
 * File:   ToneLibrary.c
 * Author: Eric Vetha
 * Brief: 
 * Created on a cloudy day
 */

#include "ToneLibrary.h"
#include <BOARD.h>
#include <xc.h>
#include <stdio.h>
#include "BNO055.h" // The header file for this source file. 
#include <math.h>

#include <stdlib.h>
#include <string.h>

// #define TONE_LIBRARY_TEST
#ifdef TONE_LIBRARY_TEST
#define DELAY(x)    {int wait; for (wait = 0; wait <= x; wait++) {asm("nop");}}
#define A_BIT       18300
#define A_LOT       183000 
#define A_TON       1830000

int main() {
    BOARD_Init();

    printf("startup_tone.mp3\n");
    printf("buffer\n");
    DELAY(A_TON);

    while (1) {
        printf("a3.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("a3.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("e4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("e4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("f4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("f4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("e4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("d4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("d4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("c4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("c4.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("b3.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("b3.mp3\n");
        printf("buffer\n");
        DELAY(A_LOT);
        printf("a3.mp3\n");
        printf("buffer\n");
        DELAY(A_TON);
    }
}
#endif
