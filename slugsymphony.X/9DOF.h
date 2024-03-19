#ifndef DOF_H
#define DOF_H

#include <stdio.h>
#include <stdlib.h>
#include <BOARD.h>
#include "BNO055.h"
#include "serial.h"
#include <sys/attribs.h>
#include <xc.h>

static int LFLAGZ;
static int LFLAGY;
static int LFLAGX;
static int RFLAGZ;
static int RFLAGY;
static int RFLAGX;

enum gravity {
    POSITIVE,
    NEGATIVE,
    NEUTRAL
};


void DOF_INIT(void);
//initialiyze the flex sensors

char InstrumentMoved();

char GuitarStrum(void);

int AnyAccell(void);
//checks to see if any major accellerometer measurments are occuring

int RAnyAccell(void);
//checks to see if any accell happens on the right hand

char CheckIMUStateLX(void);

char CheckIMUStateLY(void);

char CheckIMUStateLZ(void);

char CheckIMUStateRX(void);

char CheckIMUStateRY(void);

char CheckIMUStateRZ(void);

char DrumHit(void);

char GuitarStrum(void);

#endif 