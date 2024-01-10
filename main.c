#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mpu6050.h"
#include "registers.h"
#include "i2c.h"

int main() {
    uint8_t gyro, acc;
    uint8_t int_status = 0;
    uint8_t data[6];
    int16_t gx, gy, gz;
    float lsb_conv_gyro;
    int err = 0;

    mpu6050_t mpu6050;
    mpu6050.dev.init = i2c_init;
    mpu6050.dev.deinit = i2c_deinit;
    mpu6050.dev.read = i2c_read;
    mpu6050.dev.write = i2c_write;
    mpu6050.dev.sleep = usleep;
    
    if (mpu6050_init(&mpu6050)) {
        exit(1);
    }

    /* reset all signal paths */
    /* enable gyro, acc and temp */
    if (i2c_write(REG_SIGNAL_PATH_RESET, 0x07)) {
        exit(1);
    }

    /* set sample rate divisor to 150 */
    /* if LP-filter = 0 or 7 => sample rate = 8 Khz, otherwise 1 Khz */
    if (i2c_write(REG_SMPLRT_DIV, 150)) {
        exit(1);
    }

    /* set up digital LP-filter */
    if (i2c_write(REG_CONFIG, 0x05)) {
        exit(1);
    }

    /* set sleep to 0 */
    if (i2c_write(REG_PWR_MGMT1, 0x00)) {
        exit(1);
    }

    /* enable DATA_RDY interrupt (polling for now) */
    /* active when internal writes to all out data regs are done */
    if (i2c_write(REG_INT_ENABLE, 0x01)) {
        exit(1);
    }

    gyro = MPU6050_GYRO_FS_250 << 3;
    lsb_conv_gyro = 131.f;
    if (i2c_write(REG_GYRO_CONFIG, gyro)) {
        exit(1);
    }

    acc = MPU6050_ACC_FS_2G << 3;
    if (i2c_write(REG_ACCEL_CONFIG, acc)) {
        exit(1);
    }

    usleep(200 * 1000);

LOOP:
    do {
        err |= i2c_read(REG_INT_STATUS, &int_status, 1);
        usleep(1000);
    } while(!err && !(int_status & 1));

    if (i2c_read(REG_GYRO_XOUT_H, data, 6)) {
        exit(1);
    }

    gx = (data[0] << 8 | data[1]);
    gy = (data[2] << 8 | data[3]);
    gz = (data[4] << 8 | data[5]);

    printf("[GYRO °/s] x: % -3.4f y: %-3.4f z: %-3.4f ",
        (float)(gx)/lsb_conv_gyro,
        (float)(gy)/lsb_conv_gyro,
        (float)(gz)/lsb_conv_gyro);

    err = 0;
    int_status = 0;

    if (mpu6050_read_acc(&mpu6050)) {
        exit(1);
    }

    printf("[ACC mg] x: %-3d y: %-3d z: %-3d\n",
        mpu6050.acc.x, mpu6050.acc.y, mpu6050.acc.z);

    
goto LOOP;

    i2c_deinit();

    return 0;
}