/**
 * @file apds9960.h
 */
#ifndef APDS9960_H
#define APDS9960_H

#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>

struct apds_data {
    struct sensor_value intensity;
    struct sensor_value prox;
};

struct sensor_data {
    struct device *dev;
    struct apds_data data;
};


int apds9960_init(void);
struct apds_data apds9960_read(void);

#endif // end of APDS9960_H

