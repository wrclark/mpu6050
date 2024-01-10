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
    memset(&mpu6050->data, 0, sizeof mpu6050->data);

    return err;
}

int mpu6050_deinit(mpu6050_t *mpu6050) {
    assert(mpu6050);

    if (mpu6050->dev.deinit == NULL) {
        return 0;
    }

    return mpu6050->dev.deinit();
}

/* transform accel i16 samples st 1 lsb is 1 mg (1/1000 g) */
int mpu6050_read_acc(mpu6050_t *mpu6050) {
    uint8_t data[6];
    uint8_t shift;
    uint8_t int_status;
    int err = 0;

    assert(mpu6050);
    shift = acc_i16_shift(mpu6050->cfg.acc);

    /* if data_rdy interrupt is enabled, wait for data to be ready */
    if (mpu6050->cfg.int_enable.data_rdy) {
        do {
            err |= mpu6050->dev.read(REG_INT_STATUS, &int_status, 1);
            mpu6050->dev.sleep(1000); /* 1 ms */
        } while(!err && !(int_status & 1));
    }

    err |= mpu6050->dev.read(REG_ACCEL_XOUT_H, data, 6);
    mpu6050->data.acc.x = (int16_t)(data[0] << 8 | data[1]) >> shift;
    mpu6050->data.acc.y = (int16_t)(data[2] << 8 | data[3]) >> shift;
    mpu6050->data.acc.z = (int16_t)(data[4] << 8 | data[5]) >> shift;

    return err;
}

/* transform gyro i16 samples st 1 lsb = 0.1 deg / s */
int mpu6050_read_gyro(mpu6050_t *mpu6050) {
    uint8_t data[6];
    uint8_t shift;
    uint8_t int_status;
    int err = 0;

    assert(mpu6050);
    shift = gyro_i16_shift(mpu6050->cfg.gyro);

    /* if data_rdy interrupt is enabled, wait for data to be ready */
    if (mpu6050->cfg.int_enable.data_rdy) {
        do {
            err |= mpu6050->dev.read(REG_INT_STATUS, &int_status, 1);
            mpu6050->dev.sleep(1000); /* 1 ms */
        } while(!err && !(int_status & 1));
    }

    err |= mpu6050->dev.read(REG_GYRO_XOUT_H, data, 6);

    mpu6050->data.gyro.x = ((int16_t)(data[0] << 8 | data[1]) >> shift);
    mpu6050->data.gyro.y = ((int16_t)(data[2] << 8 | data[3]) >> shift);
    mpu6050->data.gyro.z = ((int16_t)(data[4] << 8 | data[5]) >> shift);

    return err;
}

/* transform temp i16 sample st 1 lsb = 0.1 deg C */
int mpu6050_read_temp(mpu6050_t *mpu6050) {
    uint8_t data[2];
    int err = 0;

    assert(mpu6050);

    err |= mpu6050->dev.read(REG_TEMP_OUT_H, data, 2);
    mpu6050->data.temp = (int16_t)(data[0] << 8 | data[1])/34 + 365;
    return err;
}

/* combines the operations of read_temp(), read_gyro() and read_acc() */
/* if data_rdy interrupt is enabled, this will read all data synched */
int mpu6050_read(mpu6050_t *mpu6050) {
    uint8_t data[14]; /* gyro + accel + temp */
    uint8_t shift_gyro, shift_acc, int_status;
    int err = 0;

    assert(mpu6050);

    shift_gyro = gyro_i16_shift(mpu6050->cfg.gyro);
    shift_acc = acc_i16_shift(mpu6050->cfg.acc);

    /* if data_rdy interrupt is enabled, wait for data to be ready */
    if (mpu6050->cfg.int_enable.data_rdy) {
        do {
            err |= mpu6050->dev.read(REG_INT_STATUS, &int_status, 1);
            mpu6050->dev.sleep(1000); /* 1 ms */
        } while(!err && !(int_status & 1));
    }

    err |= mpu6050->dev.read(REG_ACCEL_XOUT_H, data, 14);
    /* 0-5 acc */
    mpu6050->data.acc.x = (int16_t)(data[0] << 8 | data[1]) >> shift_acc;
    mpu6050->data.acc.y = (int16_t)(data[2] << 8 | data[3]) >> shift_acc;
    mpu6050->data.acc.z = (int16_t)(data[4] << 8 | data[5]) >> shift_acc;
    /* 6-7 temp */
    mpu6050->data.temp = (int16_t)(data[6] << 8 | data[7])/34 + 365;
    /* 8-13 gyro */
    mpu6050->data.gyro.x = ((int16_t)(data[ 8] << 8 | data[ 9]) >> shift_gyro);
    mpu6050->data.gyro.y = ((int16_t)(data[10] << 8 | data[11]) >> shift_gyro);
    mpu6050->data.gyro.z = ((int16_t)(data[12] << 8 | data[13]) >> shift_gyro);

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

/* How much to shift down an i16 gyro sample depending on full-scale mode set */
static uint8_t gyro_i16_shift(uint8_t fs) {
    switch(fs) {
        case MPU6050_GYRO_FS_250:  return 4;
        case MPU6050_GYRO_FS_500:  return 3;
        case MPU6050_GYRO_FS_1000: return 2;
        case MPU6050_GYRO_FS_2000: return 1;
        default:                   return 0;
    }
}