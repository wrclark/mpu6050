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


#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mpu6050.h"
#include "registers.h"
#include "i2c.h"

int main() {

    mpu6050_t mpu6050;

    mpu6050.dev.init = i2c_init;
    mpu6050.dev.deinit = i2c_deinit;
    mpu6050.dev.read = i2c_read;
    mpu6050.dev.write = i2c_write;
    mpu6050.dev.sleep = usleep;

    if (mpu6050_init(&mpu6050)) {
        exit(1);
    }

    if (mpu6050_reset(&mpu6050)) {
        exit(1);
    }

    if (mpu6050_calibrate_gyro(&mpu6050)) {
        exit(1);
    }

    mpu6050.cfg.gyro = MPU6050_GYRO_FS_250;
    mpu6050.cfg.acc = MPU6050_ACC_FS_2G;
    mpu6050.cfg.dlpl = 6;
    mpu6050.cfg.sdiv = 200;
    mpu6050.cfg.int_enable.data_rdy = 1;

    if (mpu6050_configure(&mpu6050)) {
        exit(1);
    }

    for ( ;; ) {

        if (mpu6050_read(&mpu6050)) {
            exit(1);
        }

        printf("[GYRO °/s] x:%4.1f  y:%4.1f  z:%4.1f ",
            (float)mpu6050.data.gyro.x / 10.f,
            (float)mpu6050.data.gyro.y / 10.f,
            (float)mpu6050.data.gyro.z / 10.f);

        printf("[ACC g] x:%4.3f  y:%4.3f  z:%4.3f ",
            (float)mpu6050.data.acc.x / 1000.f,
            (float)mpu6050.data.acc.y / 1000.f,
            (float)mpu6050.data.acc.z / 1000.f);

        printf("t:%4.1f°C\n", (float)mpu6050.data.temp / 10.f);
    }

    mpu6050_deinit(&mpu6050);

    return 0;
}