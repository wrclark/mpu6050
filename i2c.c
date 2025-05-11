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


/* 

Example I2C use on linux/raspberry pi

*/

#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdlib.h>

#include "i2c.h"

/*
 * Pinout config for this example
 *
 * MPU6050 SDA => Raspberry Pi GPIO 2 (Physical pin 3)
 * MPU6050 SCL => Raspberry Pi GPIO 3 (Physical pin 5)
 *
 */

#define I2C_DEVICE "/dev/i2c-1"
#define I2C_MPU6050_ADDRESS 0x68

static int fd;

int i2c_init(void) {
    fd = open(I2C_DEVICE, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "i2c_init(): could not open device: %s\n", I2C_DEVICE);
        return 1;
    }

    if (ioctl(fd, I2C_SLAVE, I2C_MPU6050_ADDRESS) < 0) {
        fprintf(stderr, "i2c_init(): failed to acquire bus/talk to slave\n");
        close(fd);
        return 1;
    }

    return 0;
}

int i2c_read(uint8_t reg, uint8_t *dst, uint32_t size) {

    if (write(fd, &reg, 1) != 1) {
        fprintf(stderr, "i2c_read(): error write()\n");
        return 1;
    }
    
    if (read(fd, dst, size) != (int)size) {
        fprintf(stderr, "i2c_read(): error read()\n");
        return 1;
    }

    return 0;
}

int i2c_write(uint8_t reg, uint8_t value) {
    uint8_t cmd[2];

    cmd[0] = reg;
    cmd[1] = value;
    
    if (write(fd, cmd, 2) != 2) {
        fprintf(stderr, "i2c_write(): error write()\n");
        return 1;
    }

    return 0;
}

int i2c_deinit(void) {
    if (fd) {
        close(fd);
    }

    return 0;
}