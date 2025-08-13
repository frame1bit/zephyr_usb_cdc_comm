/*
 * Copyright (c) 2019 Intel Corporation
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Sample echo app for CDC ACM class
 *
 * Sample app for USB CDC ACM class driver. The received data is echoed back
 * to the serial port.
 */

#include <sample_usbd.h>

#include <stdio.h>
#include <string.h>
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include "usb_comm.h"

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);


#define	LED0_NODE	DT_ALIAS(led0)

struct priv_data {
	uint32_t counter;
	struct gpio_dt_spec *led_dev
};

void timer_tick_cb(struct k_timer *timer)
{
	struct priv_data *data = k_timer_user_data_get(timer); 
	gpio_pin_toggle_dt(data->led_dev);
}


K_TIMER_DEFINE(timerTick, timer_tick_cb, NULL);

int main(void)
{
	struct priv_data data;
	int ret;
	struct gpio_dt_spec led_dev = GPIO_DT_SPEC_GET(LED0_NODE, gpios);
	uint32_t count = 0;

	if (!gpio_is_ready_dt(&led_dev)) {
		LOG_ERR("Failed to get GPIO DT!\n");
		return 1;
	}

	data.led_dev = &led_dev;

	ret = gpio_pin_configure_dt(&led_dev, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 1;
	}

	ret = usb_comm_init();
	if (ret < 0) {
		LOG_ERR("Failed to init usb com!\n");
		return 1;
	}

	k_timer_user_data_set(&timerTick, &data);
	k_timer_start(&timerTick, K_MSEC(500), K_MSEC(500));

	while(true) 
	{
		usb_comm_process();
		k_msleep(10);
		if (count > 100) {
			count = 0;
			data.counter = (data.counter + 1) % 100;
			usb_comm_send(0x01, 0x1234, &data.counter, 1);
		}
	}

	return 0;
}
