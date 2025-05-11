/*
 * This file is part of mpu6050.
 *
 * Copyright (C) 2025 William Clark
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */


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

    mpu6050->data.gyro.x = (int16_t)(data[0] << 8 | data[1]) >> shift;
    mpu6050->data.gyro.y = (int16_t)(data[2] << 8 | data[3]) >> shift;
    mpu6050->data.gyro.z = (int16_t)(data[4] << 8 | data[5]) >> shift;

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
    mpu6050->data.gyro.x = (int16_t)(data[8] << 8 | data[9]) >> shift_gyro;
    mpu6050->data.gyro.y = (int16_t)(data[10] << 8 | data[11]) >> shift_gyro;
    mpu6050->data.gyro.z = (int16_t)(data[12] << 8 | data[13]) >> shift_gyro;

    return err;
}

/* write configuration to device */
/* then sleep for 200 ms */
int mpu6050_configure(mpu6050_t *mpu6050) {
    uint8_t sig_path, dlpl, sleep, inten;
    uint8_t acc, gyro;
    int err = 0;
    
    assert(mpu6050);

    /* enable accel, gyro and temp */
    sig_path = 0x07;

    /* digital low-pass filter level */
    dlpl = mpu6050->cfg.dlpl & 0x07;

    /* INT_ENABLE */
    inten = mpu6050->cfg.int_enable.data_rdy & 1;
    inten |= (mpu6050->cfg.int_enable.i2c_mst & 1) << 3;
    inten |= (mpu6050->cfg.int_enable.fifo_overflow & 1) << 4;
    inten |= (mpu6050->cfg.int_enable.mot & 1) << 6;

    /* PWR_MGMT1 */
    sleep = 0;

    /* gyro */
    gyro = (mpu6050->cfg.gyro & 0x02) << 3;

    /* accelerometer */
    acc = (mpu6050->cfg.acc & 0x02) << 3;

    err |= mpu6050->dev.write(REG_SMPLRT_DIV, mpu6050->cfg.sdiv);
    err |= mpu6050->dev.write(REG_SIGNAL_PATH_RESET, sig_path);
    err |= mpu6050->dev.write(REG_INT_ENABLE, inten);
    err |= mpu6050->dev.write(REG_CONFIG, dlpl);
    err |= mpu6050->dev.write(REG_ACCEL_CONFIG, acc);
    err |= mpu6050->dev.write(REG_GYRO_CONFIG, gyro);
    err |= mpu6050->dev.write(REG_PWR_MGMT1, sleep);

    if (!err) {
        mpu6050->dev.sleep(200000); /* 200 ms */
    }

    return err;
}

/* configures the device in the recommended mode for determining calibration */
/* read gyro N times to determine a constant offset */
/* write the offsets to device, which will in turn consider them for all */
/* subsequent gyro reading operations. Assumes device is stationary. */
int mpu6050_calibrate_gyro(mpu6050_t *mpu6050) {
    uint8_t data[6];
    int err = 0;
    int i;
    int16_t x, y, z;
    uint8_t shift;
    x = y = z = 0;

    assert(mpu6050);

    /* Set clock divider to 0 (1) */
    err |= mpu6050->dev.write(REG_SMPLRT_DIV, 0);
    /* Reset gyro */
    err |= mpu6050->dev.write(REG_SIGNAL_PATH_RESET, 1 << 2);
    /* Set digital low-pass filter level to 7 */
    err |= mpu6050->dev.write(REG_CONFIG, 0);
    /* Set 1000 deg mode */
    err |= mpu6050->dev.write(REG_GYRO_CONFIG, MPU6050_GYRO_FS_1000 << 3);
    /* No sleep */
    err |= mpu6050->dev.write(REG_PWR_MGMT1, 0);

    mpu6050->dev.sleep(200000); /* 200 ms */

    shift = gyro_i16_shift(MPU6050_GYRO_FS_1000);

    for (i=0; i<MPU6050_CALIBRATION_SAMPLES; i++) {

        err |= mpu6050->dev.read(REG_GYRO_XOUT_H, data, 6);
        if (err) break;

        x = x/2 + (((int16_t)(data[0] << 8 | data[1]) >> shift) / 2);
        y = y/2 + (((int16_t)(data[2] << 8 | data[3]) >> shift) / 2);
        z = z/2 + (((int16_t)(data[4] << 8 | data[5]) >> shift) / 2);

        mpu6050->dev.sleep(1000); /* 1 ms */
    }

    /* the values expected for these registers are in the form 1 dps = 32.8 LSB */
    /* the values are subtracted, so negative drift is amplified ! */
    x = x ? -x : x;
    y = y ? -y : y;
    z = z ? -z : z;

    /* write the values to appropriate registers */
    err |= mpu6050->dev.write(REG_XG_OFF_USR_H, (x >> 8) & 0xFF);
    err |= mpu6050->dev.write(REG_XG_OFF_USR_L, x & 0xFF);
    err |= mpu6050->dev.write(REG_YG_OFF_USR_H, (y >> 8) & 0xFF);
    err |= mpu6050->dev.write(REG_YG_OFF_USR_L, y & 0xFF);
    err |= mpu6050->dev.write(REG_ZG_OFF_USR_H, (z >> 8) & 0xFF);
    err |= mpu6050->dev.write(REG_ZG_OFF_USR_L, z & 0xFF);

    return err;
}

/* simply set sleep mode */
int mpu6050_reset(mpu6050_t *mpu6050) {
    uint8_t pwrmgmt1, sigcond;
    int err = 0;

    /* enable DEVICE_RESET */
    err |= mpu6050->dev.write(REG_PWR_MGMT1, 0x80);

    /* poll PWR_MGMT1 register until MSb is 0, which means */
    /* the device reset process has finished. */
    do {
        err |= mpu6050->dev.read(REG_PWR_MGMT1, &pwrmgmt1, 1);
        mpu6050->dev.sleep(1000); /* 1 ms */
    } while (!err && pwrmgmt1 & 0x80);

    /* enable SIG_COND_RESET */
    err |= mpu6050->dev.write(REG_USER_CTRL, 0x01);

    /* poll USER_CTRL register until MSb is 0 */
    do {
        err |= mpu6050->dev.read(REG_USER_CTRL, &sigcond, 1);
        mpu6050->dev.sleep(1000); /* 1 ms */
    } while (!err && sigcond & 0x01);

    /* Overwrite old gyro offsets with 0 as they only clear on power-off */
    err |= mpu6050->dev.write(REG_XG_OFF_USR_H, 0);
    err |= mpu6050->dev.write(REG_XG_OFF_USR_L, 0);
    err |= mpu6050->dev.write(REG_YG_OFF_USR_H, 0);
    err |= mpu6050->dev.write(REG_YG_OFF_USR_L, 0);
    err |= mpu6050->dev.write(REG_ZG_OFF_USR_H, 0);
    err |= mpu6050->dev.write(REG_ZG_OFF_USR_L, 0);

    /* enable sleep mode */
    err |= mpu6050->dev.write(REG_PWR_MGMT1, 0x40);

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