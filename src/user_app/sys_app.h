#ifndef SYS_APP_H
#define SYS_APP_H


struct priv_data {
	uint32_t counter;
	struct gpio_dt_spec *led_blue;
    struct gpio_dt_spec *led_red;
    struct gpio_dt_spec *led_green;
};

/** function prototype */
int main_app_init(void *arg);
int main_app_run(void *arg);
/* end of functin prototype */

#endif /** end of SYS_APP_H */
