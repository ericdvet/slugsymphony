#include "9DOF.h"


#define movement 10 //this is the value for any major movement happening
#define GYRO_MOVEMENT_THRESHOLD 0.5
#define DRUM_HIT_THRESHOLD 8
#define GRAVITY_COEF 0.85
#define STRUM_THRESHOLD 1   
#include <math.h>

void DOF_INIT(void) {
    BNO055_A_Init(); //left hand 9dof
    BNO055_B_Init(); //right hand 9dof
}

char InstrumentMoved() {
    float current_A_GyroX = (0.00158355565428341 * (BNO055_A_ReadGyroX() - 2.94000000000000));
    float current_A_GyroY = (0.00148691821743288 * (BNO055_A_ReadGyroY() - 13.0500000000000));
    float current_A_GyroZ = (0.00148757871770715 * (BNO055_A_ReadGyroZ() + 14.9733333333333));

    float current_B_GyroX = (0.00158355565428341 * (BNO055_B_ReadGyroX() - 2.94000000000000));
    float current_B_GyroY = (0.00148691821743288 * (BNO055_B_ReadGyroY() - 13.0500000000000));
    float current_B_GyroZ = (0.00148757871770715 * (BNO055_B_ReadGyroZ() + 14.9733333333333));

    //    printf("Current A GyroX: %.6f\n", current_A_GyroX);
    //    printf("Current A GyroY: %.6f\n", current_A_GyroY);
    //    printf("Current A GyroZ: %.6f\n", current_A_GyroZ);
    ////
    //    printf("Current B GyroX: %.6f\n", current_B_GyroX);
    //    printf("Current B GyroY: %.6f\n", current_B_GyroY);
    //    printf("Current B GyroZ: %.6f\n", current_B_GyroZ);

    if (abs(current_A_GyroX) > GYRO_MOVEMENT_THRESHOLD ||
            abs(current_A_GyroY) > GYRO_MOVEMENT_THRESHOLD ||
            abs(current_A_GyroZ) > GYRO_MOVEMENT_THRESHOLD) {
        if (abs(current_B_GyroX) > GYRO_MOVEMENT_THRESHOLD ||
                abs(current_B_GyroY) > GYRO_MOVEMENT_THRESHOLD ||
                abs(current_B_GyroZ) > GYRO_MOVEMENT_THRESHOLD) {
            return 1;
        }
    }
    return 0;

}

char CheckIMUStateLX(void) {
    // Read accelerometer data for left side along X-axis
    float xVal = (0.9636 * BNO055_A_ReadAccelX() + 17.0006) / 981.0;

    // Determine orientation based on xVal
    if (xVal > GRAVITY_COEF) {
        return POSITIVE;
    } else if (xVal < -GRAVITY_COEF) {
        return NEGATIVE;
    } else {
        return NEUTRAL;
    }
}

char CheckIMUStateLY(void) {
    // Read accelerometer data for left side along Y-axis
    float yVal = (0.9757 * BNO055_A_ReadAccelY() + 27.6586) / 981.0;

    // Determine orientation based on yVal
    if (yVal > GRAVITY_COEF) {
        return POSITIVE;
    } else if (yVal < -GRAVITY_COEF) {
        return NEGATIVE;
    } else {
        return NEUTRAL;
    }
}

char CheckIMUStateLZ(void) {
    // Read accelerometer data for left side along Z-axis
    float zVal = (0.9885 * BNO055_A_ReadAccelZ() + 14.9360) / 981.0;

    // Determine orientation based on zVal
    if (zVal > GRAVITY_COEF) {
        return POSITIVE;
    } else if (zVal < -GRAVITY_COEF) {
        return NEGATIVE;
    } else {
        return NEUTRAL;
    }
}

char CheckIMUStateRX(void) {
    // Read accelerometer data for right side along X-axis
    float xVal = (0.9636 * BNO055_B_ReadAccelX() + 17.0006) / 981.0;

    // Determine orientation based on xVal
    if (xVal > GRAVITY_COEF) {
        return POSITIVE;
    } else if (xVal < -GRAVITY_COEF) {
        return NEGATIVE;
    } else {
        return NEUTRAL;
    }
}

char CheckIMUStateRY(void) {
    // Read accelerometer data for right side along Y-axis
    float yVal = (0.9757 * BNO055_B_ReadAccelY() + 27.6586) / 981.0;

    // Determine orientation based on yVal
    if (yVal > GRAVITY_COEF) {
        return POSITIVE;
    } else if (yVal < -GRAVITY_COEF) {
        return NEGATIVE;
    } else {
        return NEUTRAL;
    }
}

char CheckIMUStateRZ(void) {
    // Read accelerometer data for right side along Z-axis
    float zVal = (0.9885 * BNO055_B_ReadAccelZ() + 14.9360) / 981.0;

    // Determine orientation based on zVal
    if (zVal > GRAVITY_COEF) {
        return POSITIVE;
    } else if (zVal < -GRAVITY_COEF) {
        return NEGATIVE;
    } else {
        return NEUTRAL;
    }
}

char DrumHit(void) {
    float current_A_GyroZ = (0.00148757871770715 * (BNO055_A_ReadGyroZ() + 14.9733333333333));
    float current_B_GyroZ = (0.00148757871770715 * (BNO055_B_ReadGyroZ() + 14.9733333333333));
    char returnVal = 0;

    if (current_A_GyroZ > DRUM_HIT_THRESHOLD) {
        returnVal |= 0b10;
    }

    if (current_B_GyroZ < -DRUM_HIT_THRESHOLD) {
        returnVal |= 0b01;
    }

    return returnVal;
}

char GuitarStrum(void) {
    float current_B_GyroZ = (0.00148757871770715 * (BNO055_B_ReadGyroZ() + 14.9733333333333));
    char returnVal = 0;

//    printf("%f \n", current_B_GyroZ);
    if (current_B_GyroZ < -DRUM_HIT_THRESHOLD) {
        returnVal = 1;
    }

    return returnVal;
}

// positive on left negative on right

//int AnyAccell(void) {
//    //check the left hand
//    if (BNO055_A_ReadAccelX() == movement || BNO055_A_ReadAccelY() == movement || BNO055_A_ReadAccelZ() == movement) {
//        return 1;
//    }
//    //check the right hand
//    if (BNO055_B_ReadAccelX() == movement || BNO055_B_ReadAccelY() == movement || BNO055_B_ReadAccelZ() == movement) {
//        return 1;
//    }
//    return 0;
//}
//
//int RAnyAccell(void) {
//    //check the right hand
//    if (BNO055_B_ReadAccelX() == movement || BNO055_B_ReadAccelY() == movement || BNO055_B_ReadAccelZ() == movement) {
//        return 1;
//    }
//    return 0;
//}


