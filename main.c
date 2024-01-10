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

    mpu6050.cfg.gyro = MPU6050_GYRO_FS_2000;
    mpu6050.cfg.acc = MPU6050_ACC_FS_2G;

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

    if (i2c_write(REG_GYRO_CONFIG, mpu6050.cfg.gyro << 3)) {
        exit(1);
    }

    if (i2c_write(REG_ACCEL_CONFIG, mpu6050.cfg.acc << 3)) {
        exit(1);
    }

    usleep(200 * 1000);

LOOP:
    if (mpu6050_read_gyro(&mpu6050)) {
        exit(1);
    }

    if (mpu6050_read_acc(&mpu6050)) {
        exit(1);
    }

    printf("[GYRO °/s] x:%1.d  y:%1.d  z:%1.d ",
        mpu6050.gyro.x, mpu6050.gyro.y, mpu6050.gyro.z);

    printf("[ACC mg] x: %-3d y: %-3d z: %-3d\n",
        mpu6050.acc.x, mpu6050.acc.y, mpu6050.acc.z);

    usleep(100 * 1000);
    goto LOOP;

    i2c_deinit();

    return 0;
}