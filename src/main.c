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
#include "sys_app.h"

#ifdef CONFIG_APDS9960
#include "apds9960.h"
#endif

LOG_MODULE_REGISTER(main, LOG_LEVEL_DBG);

#define	LEDBLUE_NODE	DT_ALIAS(led0)
#define	LEDRED_NODE		DT_ALIAS(led1)

void timer_tick_cb(struct k_timer *timer)
{
	struct priv_data *data = k_timer_user_data_get(timer); 
	gpio_pin_toggle_dt(data->led_blue);
	// gpio_pin_toggle_dt(data->led_red);
}

static struct priv_data data;

K_TIMER_DEFINE(timerTick, timer_tick_cb, NULL);

int main(void)
{
	struct priv_data data;
	int ret;
	struct gpio_dt_spec led_blue = GPIO_DT_SPEC_GET(LEDBLUE_NODE, gpios);
	struct gpio_dt_spec led_red = GPIO_DT_SPEC_GET(LEDRED_NODE, gpios);

	if (!gpio_is_ready_dt(&led_blue)) {
		LOG_ERR("Failed to get GPIO DT!\n");
		return 1;
	}

	data.led_blue = &led_blue;
	data.led_red = &led_red;

	ret = gpio_pin_configure_dt(&led_blue, GPIO_OUTPUT_ACTIVE);
	if (ret < 0) {
		return 1;
	}
	ret = gpio_pin_configure_dt(&led_red, GPIO_OUTPUT_ACTIVE);
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

#ifdef CONFIG_APDS9960
	ret = apds9960_init();
	if (ret != 0) {
		LOG_ERR("Failed to init apds sensor!\n");
	}
#endif

	main_app_init(NULL);

	while(true) 
	{
		main_app_run(&data);
	}

	return 0;
}
