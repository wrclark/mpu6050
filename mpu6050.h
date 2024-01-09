#ifndef MPU6050_H
#define MPU6050_H

#include <stdint.h>

/* Gyroscope full-scale range */
#define MPU6050_GYRO_FS_250  0x00 /* ± 250 °/s */
#define MPU6050_GYRO_FS_500  0x01 /* ± 500 °/s */
#define MPU6050_GYRO_FS_1000 0x02 /* ± 1000 °/s */
#define MPU6050_GYRO_FS_2000 0x03 /* ± 2000 °/s */

/* Accelerometer full-scale range */
#define MPU6050_ACC_FS_2G    0x00 /* ± 2g */
#define MPU6050_ACC_FS_4G    0x01 /* ± 4g */
#define MPU6050_ACC_FS_8G    0x02 /* ± 8g */
#define MPU6050_ACC_FS_16G   0x03 /* ± 16g */


struct mpu6050_acc_config {
    uint8_t range;
    uint8_t filter;
};

struct mpu6050_gyro_config {
    uint8_t range;
    uint8_t filter;
};

struct mpu6050_config {
    struct mpu6050_gyro_config gyro;
    struct mpu6050_acc_config  acc;
};

struct mpu6050 {
    struct mpu6050_config cfg;
};

typedef struct mpu6050 mpu6050_t;

#endif