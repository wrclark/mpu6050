#ifndef REGISTERS_H
#define REGISTERS_H

#define REG_XA_OFF_USR_H       0x06 /* Accel X offset H */
#define REG_XA_OFF_USR_L       0x07 /* Accel X offset L */
#define REG_YA_OFF_USR_H       0x08 /* Accel Y offset H */
#define REG_YA_OFF_USR_L       0x09 /* Accel Y offset L */
#define REG_ZA_OFF_USR_H       0x0A /* Accel Z offset H */
#define REG_ZA_OFF_USR_L       0x0B /* Accel Z offset L */
#define REG_SELF_TEST_X        0x0D 
#define REG_SELF_TEST_Y        0x0E 
#define REG_SELF_TEST_Z        0x0F 
#define REG_SELF_TEST_A        0x10 
#define REG_XG_OFF_USR_H       0x13 /* Gyro X offset H */
#define REG_XG_OFF_USR_L       0x14 /* Gyro X offset L */
#define REG_YG_OFF_USR_H       0x15 /* Gyro Y offset H */
#define REG_YG_OFF_USR_L       0x16 /* Gyro Y offset L */
#define REG_ZG_OFF_USR_H       0x17 /* Gyro Z offset H */
#define REG_ZG_OFF_USR_L       0x18 /* Gyro Z offset L */
#define REG_SMPLRT_DIV         0x19 
#define REG_CONFIG             0x1A 
#define REG_GYRO_CONFIG        0x1B 
#define REG_ACCEL_CONFIG       0x1C 
#define REG_MOT_THR            0x1F
#define REG_FIFO_EN            0x23 
#define REG_I2C_MST_CTRL       0x24
#define REG_I2C_SLV0_ADDR      0x25
#define REG_I2C_SLV0_REG       0x26
#define REG_I2C_SLV0_CTRL      0x27
#define REG_I2C_SLV1_ADDR      0x28
#define REG_I2C_SLV1_REG       0x29
#define REG_I2C_SLV1_CTRL      0x2A
#define REG_I2C_SLV2_ADDR      0x2B
#define REG_I2C_SLV2_REG       0x2C
#define REG_I2C_SLV2_CTRL      0x2D
#define REG_I2C_SLV3_ADDR      0x2E
#define REG_I2C_SLV3_REG       0x2F
#define REG_I2C_SLV3_CTRL      0x30
#define REG_I2C_SLV4_ADDR      0x31
#define REG_I2C_SLV4_REG       0x32
#define REG_I2C_SLV4_DO        0x33
#define REG_I2C_SLV4_CTRL      0x34
#define REG_I2C_SLV4_DI        0x35
#define REG_I2C_MST_STATUS     0x36
#define REG_INT_PIN_CFG        0x37
#define REG_INT_ENABLE         0x38
#define REG_INT_STATUS         0x3A
#define REG_ACCEL_XOUT_H       0x3B
#define REG_ACCEL_XOUT_L       0x3C
#define REG_ACCEL_YOUT_H       0x3D
#define REG_ACCEL_YOUT_L       0x3E
#define REG_ACCEL_ZOUT_H       0x3F
#define REG_ACCEL_ZOUT_L       0x40
#define REG_TEMP_OUT_H         0x41
#define REG_TEMP_OUT_L         0x42
#define REG_GYRO_XOUT_H        0x43
#define REG_GYRO_XOUT_L        0x44
#define REG_GYRO_YOUT_H        0x45
#define REG_GYRO_YOUT_L        0x46
#define REG_GYRO_ZOUT_H        0x47
#define REG_GYRO_ZOUT_L        0x48
#define EXT_SENS_DATA_00       0x49
#define EXT_SENS_DATA_01       0x4A
#define EXT_SENS_DATA_02       0x4B
#define EXT_SENS_DATA_03       0x4C
#define EXT_SENS_DATA_04       0x4D
#define EXT_SENS_DATA_05       0x4E
#define EXT_SENS_DATA_06       0x4F
#define EXT_SENS_DATA_07       0x50
#define EXT_SENS_DATA_08       0x51
#define EXT_SENS_DATA_09       0x52
#define EXT_SENS_DATA_10       0x53
#define EXT_SENS_DATA_11       0x54
#define EXT_SENS_DATA_12       0x55
#define EXT_SENS_DATA_13       0x56
#define EXT_SENS_DATA_14       0x57
#define EXT_SENS_DATA_15       0x58
#define EXT_SENS_DATA_16       0x59
#define EXT_SENS_DATA_17       0x5A
#define EXT_SENS_DATA_18       0x5B
#define EXT_SENS_DATA_19       0x5C
#define EXT_SENS_DATA_20       0x5D
#define EXT_SENS_DATA_21       0x5E
#define EXT_SENS_DATA_22       0x5F
#define EXT_SENS_DATA_23       0x60
#define REG_I2C_SLV0_DO        0x63
#define REG_I2C_SLV1_DO        0x64
#define REG_I2C_SLV2_DO        0x65
#define REG_I2C_SLV3_DO        0x66
#define REG_I2C_MST_DELAY_CTRL 0x67
#define REG_SIGNAL_PATH_RESET  0x68
#define REG_MOT_DETECT_CTRL    0x69
#define REG_USER_CTRL          0x6A
#define REG_PWR_MGMT1          0x6B
#define REG_PWR_MGMT2          0x6C
#define REG_FIFO_COUNT_H       0x72
#define REG_FIFO_COUNT_L       0x73
#define REG_FIFO_R_W           0x74
#define REG_WHO_AM_I           0x75

#endif