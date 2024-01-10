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