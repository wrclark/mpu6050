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

struct mpu6050_dev {
    int (*init)(void);
    int (*write)(uint8_t reg, uint8_t value);
    int (*read)(uint8_t reg, uint8_t *dst, uint32_t size);
    int (*sleep)(uint32_t dur_us);
    int (*deinit)(void);
};

/* for configuring REG_INT_ENABLE */
struct mpu6050_int_enable {
    uint8_t mot; /* enables motion detect interrupt */
    uint8_t fifo_overflow; /* enables FIFO buffer overflow interrupt */
    uint8_t i2c_mst; /* enables any i2c master to generate interrupt */
    uint8_t data_rdy; /* enable interrupt upon finished write to data out registers */
};

/* configuring variables that are written to the device */
struct mpu6050_config {
    uint8_t gyro;
    uint8_t acc;
    struct mpu6050_int_enable int_enable;
};

/* 1 lsb = 1 mg (1/1000 g) */
struct mpu6050_accelerometer {
    int16_t x;
    int16_t y;
    int16_t z;
};

/* 1 lsb = 0.1 deg / s */
struct mpu6050_gyroscope {
    int16_t x;
    int16_t y;
    int16_t z;
};

/* stores all converted data read from the device */
struct mpu6050_data {
    struct mpu6050_accelerometer acc;
    struct mpu6050_gyroscope gyro;
    int16_t temp;
};

struct mpu6050 {
    struct mpu6050_dev dev;
    struct mpu6050_config cfg;
    struct mpu6050_data data;
};

typedef struct mpu6050 mpu6050_t;

int mpu6050_init(mpu6050_t *mpu6050);
int mpu6050_deinit(mpu6050_t *mpu6050);
int mpu6050_read_acc(mpu6050_t *mpu6050);
int mpu6050_read_gyro(mpu6050_t *mpu6050);
int mpu6050_read_temp(mpu6050_t *mpu6050);
int mpu6050_read(mpu6050_t *mpu6050);

#endif