/* 
 * File:   BNO055.c
 * Author: Aaron Hunter
 * Brief: 
 * Created on April 11, 2022 9:07 am
 * Modified on <month> <day>, <year>, <hour> <pm/am>
 */

/*******************************************************************************
 * #INCLUDES                                                                   *
 ******************************************************************************/

#include <I2C.h>
#include <BOARD.h>
#include <xc.h>
#include <stdio.h>
#include "BNO055.h" // The header file for this source file. 
#include <math.h>

/*******************************************************************************
 * PRIVATE #DEFINES                                                            *
 ******************************************************************************/
/** BNO055 Address A: when ADR (COM3) pin is tied to ground (default) **/
#define BNO055_ADDRESS_A (0x28)
/** BNO055 Address B: when ADR (COM3) pin is tied to +3.3V **/
#define BNO055_ADDRESS_B (0x29)
/** BNO055 ID **/
#define BNO055_ID (0xA0)
/*Page numbers*/
#define BNO055_PAGE0 0
#define BNO055_PAGE1 1
/** Sensor configuration values: 
 * ACC_PWR_Mode <2:0> ACC_BW <2:0> ACC_Range <1:0>
 * 64Hz [GYR_Config_0]: xx110xxxb | 250 dps [GYR_Config_0]: xxxxx011b**/
#define ACC_CONFIG_PARAMS (0x18) //+/-2g, 62.5 Hz BW
#define GYRO_CONFIG_PARAMS_0 (0x33)
#define UNITS_PARAM (0x001)


/*******************************************************************************
 * PRIVATE TYPEDEFS                                                            *
 ******************************************************************************/

/*Register address copied from Adafruit Github
 * https://github.com/adafruit/Adafruit_BNO055/blob/master/Adafruit_BNO055.h    *
 */
static enum {
    /* PAGE0 REGISTER DEFINITION START*/
    BNO055_CHIP_ID_ADDR = 0x00,
    BNO055_ACCEL_REV_ID_ADDR = 0x01,
    BNO055_MAG_REV_ID_ADDR = 0x02,
    BNO055_GYRO_REV_ID_ADDR = 0x03,
    BNO055_SW_REV_ID_LSB_ADDR = 0x04,
    BNO055_SW_REV_ID_MSB_ADDR = 0x05,
    BNO055_BL_REV_ID_ADDR = 0X06,

    /* Page id register definition */
    BNO055_PAGE_ID_ADDR = 0X07,

    /* Accel data register */
    BNO055_ACCEL_DATA_X_LSB_ADDR = 0X08,
    BNO055_ACCEL_DATA_X_MSB_ADDR = 0X09,
    BNO055_ACCEL_DATA_Y_LSB_ADDR = 0X0A,
    BNO055_ACCEL_DATA_Y_MSB_ADDR = 0X0B,
    BNO055_ACCEL_DATA_Z_LSB_ADDR = 0X0C,
    BNO055_ACCEL_DATA_Z_MSB_ADDR = 0X0D,

    /* Mag data register */
    BNO055_MAG_DATA_X_LSB_ADDR = 0X0E,
    BNO055_MAG_DATA_X_MSB_ADDR = 0X0F,
    BNO055_MAG_DATA_Y_LSB_ADDR = 0X10,
    BNO055_MAG_DATA_Y_MSB_ADDR = 0X11,
    BNO055_MAG_DATA_Z_LSB_ADDR = 0X12,
    BNO055_MAG_DATA_Z_MSB_ADDR = 0X13,

    /* Gyro data registers */
    BNO055_GYRO_DATA_X_LSB_ADDR = 0X14,
    BNO055_GYRO_DATA_X_MSB_ADDR = 0X15,
    BNO055_GYRO_DATA_Y_LSB_ADDR = 0X16,
    BNO055_GYRO_DATA_Y_MSB_ADDR = 0X17,
    BNO055_GYRO_DATA_Z_LSB_ADDR = 0X18,
    BNO055_GYRO_DATA_Z_MSB_ADDR = 0X19,

    /* Euler data registers */
    BNO055_EULER_X_LSB_ADDR = 0X1A,
    BNO055_EULER_X_MSB_ADDR = 0X1B,
    BNO055_EULER_Y_LSB_ADDR = 0X1C,
    BNO055_EULER_Y_MSB_ADDR = 0X1D,
    BNO055_EULER_Z_LSB_ADDR = 0X1E,
    BNO055_EULER_Z_MSB_ADDR = 0X1F,

    /* Quaternion data registers */
    BNO055_QUATERNION_DATA_W_LSB_ADDR = 0X20,
    BNO055_QUATERNION_DATA_W_MSB_ADDR = 0X21,
    BNO055_QUATERNION_DATA_X_LSB_ADDR = 0X22,
    BNO055_QUATERNION_DATA_X_MSB_ADDR = 0X23,
    BNO055_QUATERNION_DATA_Y_LSB_ADDR = 0X24,
    BNO055_QUATERNION_DATA_Y_MSB_ADDR = 0X25,
    BNO055_QUATERNION_DATA_Z_LSB_ADDR = 0X26,
    BNO055_QUATERNION_DATA_Z_MSB_ADDR = 0X27,

    /* Linear acceleration data registers */
    BNO055_LINEAR_ACCEL_DATA_X_LSB_ADDR = 0X28,
    BNO055_LINEAR_ACCEL_DATA_X_MSB_ADDR = 0X29,
    BNO055_LINEAR_ACCEL_DATA_Y_LSB_ADDR = 0X2A,
    BNO055_LINEAR_ACCEL_DATA_Y_MSB_ADDR = 0X2B,
    BNO055_LINEAR_ACCEL_DATA_Z_LSB_ADDR = 0X2C,
    BNO055_LINEAR_ACCEL_DATA_Z_MSB_ADDR = 0X2D,

    /* Gravity data registers */
    BNO055_GRAVITY_DATA_X_LSB_ADDR = 0X2E,
    BNO055_GRAVITY_DATA_X_MSB_ADDR = 0X2F,
    BNO055_GRAVITY_DATA_Y_LSB_ADDR = 0X30,
    BNO055_GRAVITY_DATA_Y_MSB_ADDR = 0X31,
    BNO055_GRAVITY_DATA_Z_LSB_ADDR = 0X32,
    BNO055_GRAVITY_DATA_Z_MSB_ADDR = 0X33,

    /* Temperature data register */
    BNO055_TEMP_ADDR = 0X34,

    /* Status registers */
    BNO055_CALIB_STAT_ADDR = 0X35,
    BNO055_SELFTEST_RESULT_ADDR = 0X36,
    BNO055_INTR_STAT_ADDR = 0X37,

    BNO055_SYS_CLK_STAT_ADDR = 0X38,
    BNO055_SYS_STAT_ADDR = 0X39,
    BNO055_SYS_ERR_ADDR = 0X3A,

    /* Unit selection register */
    BNO055_UNIT_SEL_ADDR = 0X3B,

    /* Mode registers */
    BNO055_OPR_MODE_ADDR = 0X3D,
    BNO055_PWR_MODE_ADDR = 0X3E,

    BNO055_SYS_TRIGGER_ADDR = 0X3F,
    BNO055_TEMP_SOURCE_ADDR = 0X40,

    /* Axis remap registers */
    BNO055_AXIS_MAP_CONFIG_ADDR = 0X41,
    BNO055_AXIS_MAP_SIGN_ADDR = 0X42,

    /* SIC registers */
    BNO055_SIC_MATRIX_0_LSB_ADDR = 0X43,
    BNO055_SIC_MATRIX_0_MSB_ADDR = 0X44,
    BNO055_SIC_MATRIX_1_LSB_ADDR = 0X45,
    BNO055_SIC_MATRIX_1_MSB_ADDR = 0X46,
    BNO055_SIC_MATRIX_2_LSB_ADDR = 0X47,
    BNO055_SIC_MATRIX_2_MSB_ADDR = 0X48,
    BNO055_SIC_MATRIX_3_LSB_ADDR = 0X49,
    BNO055_SIC_MATRIX_3_MSB_ADDR = 0X4A,
    BNO055_SIC_MATRIX_4_LSB_ADDR = 0X4B,
    BNO055_SIC_MATRIX_4_MSB_ADDR = 0X4C,
    BNO055_SIC_MATRIX_5_LSB_ADDR = 0X4D,
    BNO055_SIC_MATRIX_5_MSB_ADDR = 0X4E,
    BNO055_SIC_MATRIX_6_LSB_ADDR = 0X4F,
    BNO055_SIC_MATRIX_6_MSB_ADDR = 0X50,
    BNO055_SIC_MATRIX_7_LSB_ADDR = 0X51,
    BNO055_SIC_MATRIX_7_MSB_ADDR = 0X52,
    BNO055_SIC_MATRIX_8_LSB_ADDR = 0X53,
    BNO055_SIC_MATRIX_8_MSB_ADDR = 0X54,

    /* Accelerometer Offset registers */
    ACCEL_OFFSET_X_LSB_ADDR = 0X55,
    ACCEL_OFFSET_X_MSB_ADDR = 0X56,
    ACCEL_OFFSET_Y_LSB_ADDR = 0X57,
    ACCEL_OFFSET_Y_MSB_ADDR = 0X58,
    ACCEL_OFFSET_Z_LSB_ADDR = 0X59,
    ACCEL_OFFSET_Z_MSB_ADDR = 0X5A,

    /* Magnetometer Offset registers */
    MAG_OFFSET_X_LSB_ADDR = 0X5B,
    MAG_OFFSET_X_MSB_ADDR = 0X5C,
    MAG_OFFSET_Y_LSB_ADDR = 0X5D,
    MAG_OFFSET_Y_MSB_ADDR = 0X5E,
    MAG_OFFSET_Z_LSB_ADDR = 0X5F,
    MAG_OFFSET_Z_MSB_ADDR = 0X60,

    /* Gyroscope Offset register s*/
    GYRO_OFFSET_X_LSB_ADDR = 0X61,
    GYRO_OFFSET_X_MSB_ADDR = 0X62,
    GYRO_OFFSET_Y_LSB_ADDR = 0X63,
    GYRO_OFFSET_Y_MSB_ADDR = 0X64,
    GYRO_OFFSET_Z_LSB_ADDR = 0X65,
    GYRO_OFFSET_Z_MSB_ADDR = 0X66,

    /* Radius registers */
    ACCEL_RADIUS_LSB_ADDR = 0X67,
    ACCEL_RADIUS_MSB_ADDR = 0X68,
    MAG_RADIUS_LSB_ADDR = 0X69,
    MAG_RADIUS_MSB_ADDR = 0X6A
} BNO055_P0_REGISTERS;

static enum {
    /* PAGE1 REGISTER DEFINITION START*/
    /*sensor configurations*/
    BNO055_ACC_CONFIG = 0x08,
    BNO055_MAG_CONFIG = 0x09,
    BNO055_GYR_CONFIG_0 = 0x0A,
    BNO055_GYR_CONFIG_1 = 0x0B,
    BNO055_ACC_SLEEP_CONFIG = 0x0C,
    BNO055_GYR_SLEEP_CONFIG = 0x0D,
    BNO055_INT_MSK = 0x0F,
    BNO055_INT_EN = 0x10,
    BNO055_ACC_AM_THRES = 0x11,
    BNO055_ACC_INT_SETTINGS = 0x12,
    BNO055_ACC_HG_DURATION = 0x13,
    BNO055_ACC_HG_THRES = 0x14,
    BNO055_ACC_NM_THRES = 0x15,
    BNO055_ACC_NM_SET = 0x16,
    BNO055_GYR_INT_SETTINGS = 0x17,
    BNO055_GYR_HR_X_SET = 0x18,
    BNO055_GYR_DUR_X = 0x19,
    BNO055_GYR_HR_Y_SET = 0x1A,
    BNO055_GYR_DUR_Y = 0x1B,
    BNO055_GYR_HR_Z_SET = 0x1C,
    BNO055_GYR_DUR_Z = 0x1D,
    BNO055_GYR_AM_THRES = 0x1E,
    BNO055_GYR_AM_SET = 0x1F
} BNO055_P1_REGISTERS;

/** BNO055 power settings */
static enum {
    POWER_MODE_NORMAL = 0X00,
    POWER_MODE_LOWPOWER = 0X01,
    POWER_MODE_SUSPEND = 0X02
} BNO055_powermode;

/** Operation mode settings **/
static enum {
    OPERATION_MODE_CONFIG = 0X00,
    OPERATION_MODE_ACCONLY = 0X01,
    OPERATION_MODE_MAGONLY = 0X02,
    OPERATION_MODE_GYRONLY = 0X03,
    OPERATION_MODE_ACCMAG = 0X04,
    OPERATION_MODE_ACCGYRO = 0X05,
    OPERATION_MODE_MAGGYRO = 0X06,
    OPERATION_MODE_AMG = 0X07,
    OPERATION_MODE_IMUPLUS = 0X08,
    OPERATION_MODE_COMPASS = 0X09,
    OPERATION_MODE_M4G = 0X0A,
    OPERATION_MODE_NDOF_FMC_OFF = 0X0B,
    OPERATION_MODE_NDOF = 0X0C
} BNO055_opmode;

#define QUATERNION_LSB 8192.0f // 2^14

/*******************************************************************************
 * PRIVATE FUNCTIONS PROTOTYPES                                                 *
 ******************************************************************************/
void DelayMicros(uint32_t microsec);
/*******************************************************************************
 * PUBLIC FUNCTION IMPLEMENTATIONS                                             *
 ******************************************************************************/

/**
 * @Function BNO055_Init(void)

 * @return 0 if error, 1 if succeeded
 * @brief  Initializes the BNO055 for usage. Sensors will be at Accel: 2g,Gyro:  250dps
 * @author Aaron Hunter */
char BNO055_A_Init(void) {
    unsigned int intReturn;
    unsigned char byteReturn;
    DelayMicros(250000 << 2); // delaying to ensure that successive programs do not glitch the sensor

    intReturn = I2C_Init(I2C_DEFAULT_RATE);
    if (intReturn != I2C_DEFAULT_RATE) {
        return FALSE;
    }

    I2C1CONbits.PEN = 1; // after the delay we are also going to send a stop condition to supposedly clear the bus
    while (I2C1CONbits.PEN == 1);
    /* Read chip ID to verify sensor connection */
    byteReturn = I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_CHIP_ID_ADDR);
    if (byteReturn != BNO055_ID) {
        return (FALSE);
    }
    /* default state is in CONFIG_MODE. This is the only mode in which all the 
     * writable register map entries can be changed. (Exceptions from this rule
     *  are the interrupt registers (INT and INT_MSK) and the operation mode
     *  register (OPR_MODE), which can be modified in any operation mode.)*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG);
    DelayMicros(250000); // delay between changing op modes > 19 msec
    /*set the register page to page 1*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_PAGE_ID_ADDR, BNO055_PAGE1);
    /* Config gyro for 250 dps */
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_GYR_CONFIG_0, GYRO_CONFIG_PARAMS_0);
    /* Config accelerometer to +/- 2g */
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_ACC_CONFIG, ACC_CONFIG_PARAMS);
    /*set the register page to page 0*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_PAGE_ID_ADDR, BNO055_PAGE0);
    /*set units*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_UNIT_SEL_ADDR, UNITS_PARAM);
    /* Set operation mode to AMG */
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_A, BNO055_OPR_MODE_ADDR, OPERATION_MODE_NDOF);
    return (byteReturn);

}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadAccelX() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_ACCEL_DATA_X_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadAccelY() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_ACCEL_DATA_Y_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadAccelZ() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_ACCEL_DATA_Z_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadGyroX() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_GYRO_DATA_X_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadGyroY() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_GYRO_DATA_Y_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadGyroZ() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_GYRO_DATA_Z_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadMagX() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_MAG_DATA_X_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadMagY() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_MAG_DATA_Y_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadMagZ() {
    return (I2C_ReadInt(BNO055_ADDRESS_A, BNO055_MAG_DATA_Z_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadTemp() {
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_TEMP_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadEulX(void) { // heading??
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_EULER_X_MSB_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadEulY(void) { // roll
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_EULER_Y_LSB_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadEulZ(void) { // pitch
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_EULER_Y_LSB_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadQuatW(void) { // heading??
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_QUATERNION_DATA_W_LSB_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadQuatX(void) { // roll
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_QUATERNION_DATA_X_LSB_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadQuatY(void) { // pitch
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_QUATERNION_DATA_Y_LSB_ADDR));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_A_ReadQuatZ(void) { // pitch
    return (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_QUATERNION_DATA_Z_LSB_ADDR));
}

/**
 * @Function BNO055_Init(void)

 * @return 0 if error, 1 if succeeded
 * @brief  Initializes the BNO055 for usage. Sensors will be at Accel: 2g,Gyro:  250dps
 * @author Aaron Hunter */
char BNO055_B_Init(void) {
    unsigned int intReturn;
    unsigned char byteReturn;
    DelayMicros(250000 << 2); // delaying to ensure that successive programs do not glitch the sensor

    //        intReturn = I2C_Init(I2C_DEFAULT_RATE);
    //        if (intReturn != I2C_DEFAULT_RATE) {
    //            return FALSE;
    //        }

    I2C1CONbits.PEN = 1; // after the delay we are also going to send a stop condition to supposedly clear the bus
    while (I2C1CONbits.PEN == 1);
    /* Read chip ID to verify sensor connection */
    byteReturn = I2C_ReadRegister(BNO055_ADDRESS_B, BNO055_CHIP_ID_ADDR);
    if (byteReturn != BNO055_ID) {
        return (FALSE);
    }
    /* default state is in CONFIG_MODE. This is the only mode in which all the 
     * writable register map entries can be changed. (Exceptions from this rule
     *  are the interrupt registers (INT and INT_MSK) and the operation mode
     *  register (OPR_MODE), which can be modified in any operation mode.)*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_OPR_MODE_ADDR, OPERATION_MODE_CONFIG);
    DelayMicros(250000); // delay between changing op modes > 19 msec
    /*set the register page to page 1*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_PAGE_ID_ADDR, BNO055_PAGE1);
    /* Config gyro for 250 dps */
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_GYR_CONFIG_0, GYRO_CONFIG_PARAMS_0);
    /* Config accelerometer to +/- 2g */
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_ACC_CONFIG, ACC_CONFIG_PARAMS);
    /*set the register page to page 0*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_PAGE_ID_ADDR, BNO055_PAGE0);
    /*set units*/
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_UNIT_SEL_ADDR, UNITS_PARAM);
    /* Set operation mode to AMG */
    byteReturn = I2C_WriteReg(BNO055_ADDRESS_B, BNO055_OPR_MODE_ADDR, OPERATION_MODE_NDOF);
    return (byteReturn);

}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadAccelX() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_ACCEL_DATA_X_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadAccelY() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_ACCEL_DATA_Y_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadAccelZ() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_ACCEL_DATA_Z_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadGyroX() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_GYRO_DATA_X_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadGyroY() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_GYRO_DATA_Y_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadGyroZ() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_GYRO_DATA_Z_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadMagX() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_MAG_DATA_X_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadMagY() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_MAG_DATA_Y_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadMagZ() {
    return (I2C_ReadInt(BNO055_ADDRESS_B, BNO055_MAG_DATA_Z_LSB_ADDR, 0));
}

/**
 * @Function BNO055_Read*()
 * @param None
 * @return Returns raw sensor reading
 * @brief reads sensor axis as given by name
 * @author Aaron Hunter*/
int BNO055_B_ReadTemp() {
    return (I2C_ReadRegister(BNO055_ADDRESS_B, BNO055_TEMP_ADDR));
}

#include <math.h>

//#define LSB_QUATERNION 16384.0f // 2^14
#define LSB_QUATERNION 1.0f
#define RAD_TO_DEG 57.2957795131f // Conversion factor from radians to degrees

float BNO055_B_ReadQuatW(void) {
    return (float) (I2C_ReadRegister(BNO055_ADDRESS_B, BNO055_QUATERNION_DATA_W_LSB_ADDR)) / LSB_QUATERNION;
}

float BNO055_B_ReadQuatX(void) {
    return (float) (I2C_ReadRegister(BNO055_ADDRESS_B, BNO055_QUATERNION_DATA_X_LSB_ADDR)) / LSB_QUATERNION;
}

float BNO055_B_ReadQuatY(void) {
    return (float) (I2C_ReadRegister(BNO055_ADDRESS_B, BNO055_QUATERNION_DATA_Y_LSB_ADDR)) / LSB_QUATERNION;
}

float BNO055_B_ReadQuatZ(void) {
    return (float) (I2C_ReadRegister(BNO055_ADDRESS_B, BNO055_QUATERNION_DATA_Z_LSB_ADDR)) / LSB_QUATERNION;
}

float RollFromQuaternion() {
    float qw = BNO055_B_ReadQuatW();
    float qx = BNO055_B_ReadQuatX();
    float qy = BNO055_B_ReadQuatY();

    // roll (x-axis rotation)
    float sinr_cosp = 2.0f * (qw * qx + qy * BNO055_B_ReadQuatZ());
    float cosr_cosp = 1.0f - 2.0f * (qx * qx + qy * qy);
    float roll = atan2f(sinr_cosp, cosr_cosp) * RAD_TO_DEG;
    return roll;
}

float PitchFromQuaternion() {
    float qw = BNO055_B_ReadQuatW();
    float qx = BNO055_B_ReadQuatX();
    float qy = BNO055_B_ReadQuatY();
    float qz = BNO055_B_ReadQuatZ();

    // pitch (y-axis rotation)
    float sinp = 2.0f * (qw * qy - qz * qx);
    float pitch;
    if (fabsf(sinp) >= 1)
        pitch = copysignf(90.0f, sinp); // use 90 degrees if out of range
    else
        pitch = asinf(sinp) * RAD_TO_DEG;
    return pitch;
}

float YawFromQuaternion() {
    float qw = BNO055_B_ReadQuatW();
    float qx = BNO055_B_ReadQuatX();
    float qy = BNO055_B_ReadQuatY();
    float qz = BNO055_B_ReadQuatZ();

    // yaw (z-axis rotation)
    float siny_cosp = 2.0f * (qw * qz + qx * qy);
    float cosy_cosp = 1.0f - 2.0f * (qy * qy + qz * qz);
    float yaw = atan2f(siny_cosp, cosy_cosp) * RAD_TO_DEG;
    return yaw;
}

/*******************************************************************************
 * PRIVATE FUNCTION IMPLEMENTATIONS                                            *
 ******************************************************************************/

void DelayMicros(uint32_t microsec) {
    uint32_t tWait, tStart, tCurrent;

    // Calculate the amount of wait time in terms of core processor frequency.
    tWait = (80000000L / 2000000) * microsec;
    asm volatile("mfc0   %0, $9" : "=r"(tStart));
    tCurrent = tStart;
    while ((tCurrent - tStart) < tWait) {
        asm volatile("mfc0   %0, $9" : "=r"(tCurrent));
    }// wait for the time to pass
}

// #define BNO055_TEST
#ifdef BNO055_TEST

#include "serial.h"
#define POWERPIN_LAT LATFbits.LATF1
#define POWERPIN_TRIS TRISFbits.TRISF1

int main(void) {
    char initResultA, initResultB;
    BOARD_Init();

    printf("Welcome to the BNO055 test compiled at " __DATE__ " " __TIME__ ". Sensor will be brought up and then values displayed\r\n");
    while (!IsTransmitEmpty());
    POWERPIN_LAT = 0;
    POWERPIN_TRIS = 0;
    POWERPIN_LAT = 1;

    DelayMicros(100000);
    initResultA = BNO055_A_Init();
    initResultB = BNO055_B_Init();
    

    printf("%d %d\n", initResultA, initResultB);
    if (initResultA != TRUE || initResultB != TRUE) {
        printf("Initialization of IMU failed, stopping here\r\n");
    } else {
        printf("Initialization succeeded\r\n");
        while (1) {
            if (IsTransmitEmpty()) {
                //                printf("Gyro B: (%+06d, %+06d, %+06d, %+06d)   ", BNO055_B_ReadQuatW(), BNO055_B_ReadQuatX(),
                //                        BNO055_B_ReadQuatY(), BNO055_B_ReadQuatW());
                //                printf("Oiler Angles B: (%f, %f, %f)   ", RollFromQuaternion(), PitchFromQuaternion(), YawFromQuaternion());
                //                                printf("%d\n", (I2C_ReadRegister(BNO055_ADDRESS_A, BNO055_CALIB_STAT_ADDR) >> 6) & 0x03);
                //                printf("Accel: (%+06d, %+06d, %+06d)   ", BNO055_B_ReadAccelX(), BNO055_B_ReadAccelY(), BNO055_B_ReadAccelZ());
                printf("Mag: (%+06d, %+06d, %+06d)   ", BNO055_B_ReadMagX(), BNO055_B_ReadMagY(), BNO055_B_ReadMagZ());
                //                                printf("Temp: %+06d", BNO055_A_ReadTemp());
                printf("\r\n");
                DelayMicros(100000);
            }
        }
    }
    while (1);
}

#endif