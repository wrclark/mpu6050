#include <stddef.h>
#include <string.h>
#include <assert.h>
#include "mpu6050.h"
#include "registers.h"

static uint8_t acc_i16_shift(uint8_t fs);
static uint8_t gyro_i16_shift(uint8_t fs);

int mpu6050_init(mpu6050_t *mpu6050) {
    uint8_t id;
    int err = 0;

    assert(mpu6050);

    if (mpu6050->dev.init != NULL) {
        if (mpu6050->dev.init() != 0) {
            return 1;
        }
    }

    err |= mpu6050->dev.read(REG_WHO_AM_I, &id, 1);
    if (id != 0x68) {
        return 1;
    }
    
    memset(&mpu6050->cfg, 0, sizeof mpu6050->cfg);
    memset(&mpu6050->acc, 0, sizeof mpu6050->acc);
    memset(&mpu6050->gyro, 0, sizeof mpu6050->gyro);

    return err;
}

int mpu6050_deinit(mpu6050_t *mpu6050) {
    assert(mpu6050);

    if (mpu6050->dev.deinit == NULL) {
        return 0;
    }

    return mpu6050->dev.deinit();
}

/* transform i16 samples st 1 lsb is 1 mg (1/1000 g) */
int mpu6050_read_acc(mpu6050_t *mpu6050) {
    uint8_t data[6];
    uint8_t shift;
    int err = 0;

    assert(mpu6050);

    shift = acc_i16_shift(mpu6050->cfg.acc);

    err |= mpu6050->dev.read(REG_ACCEL_XOUT_H, data, 6);
    mpu6050->acc.x = (int16_t)(data[0] << 8 | data[1]) >> shift;
    mpu6050->acc.y = (int16_t)(data[2] << 8 | data[3]) >> shift;
    mpu6050->acc.z = (int16_t)(data[4] << 8 | data[5]) >> shift;

    return err;
}

/* transform gyro i16 sample st 1 lsb = 0.1 deg / s */
int mpu6050_read_gyro(mpu6050_t *mpu6050) {
    uint8_t data[6];
    uint8_t shift;
    int err = 0;

    assert(mpu6050);

    shift = gyro_i16_shift(mpu6050->cfg.gyro);

    err |= mpu6050->dev.read(REG_GYRO_XOUT_H, data, 6);

    mpu6050->gyro.x = ((int16_t)(data[0] << 8 | data[1]) >> shift);
    mpu6050->gyro.y = ((int16_t)(data[2] << 8 | data[3]) >> shift);
    mpu6050->gyro.z = ((int16_t)(data[4] << 8 | data[5]) >> shift);

    return err;
}

/* How much to shift down an i16 accel sample depending on full-scale mode set */
static uint8_t acc_i16_shift(uint8_t fs) {
    switch(fs) {
        case MPU6050_ACC_FS_2G:  return 4; /* 16384 LSb / g */
        case MPU6050_ACC_FS_4G:  return 3; /* 8192 LSb / g */
        case MPU6050_ACC_FS_8G:  return 2; /* 4096 LSb / g */
        case MPU6050_ACC_FS_16G: return 1; /* 2048 LSb / g */
        default:                 return 0;
    }
}

/* How much to shift down an i16 sample depending on full-scale mode set */
static uint8_t gyro_i16_shift(uint8_t fs) {
    switch(fs) {
        case MPU6050_GYRO_FS_250:  return 4;
        case MPU6050_GYRO_FS_500:  return 3;
        case MPU6050_GYRO_FS_1000: return 2;
        case MPU6050_GYRO_FS_2000: return 1;
        default:                   return 0;
    }
}