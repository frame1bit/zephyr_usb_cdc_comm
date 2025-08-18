#include <zephyr/drivers/sensor.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
#include "apds9960.h"
LOG_MODULE_REGISTER(apds9960, LOG_LEVEL_DBG);

static struct sensor_data apds;

#ifdef CONFIG_APDS9960_TRIGGER
K_SEM_DEFINE(sem, 0, 1);

static void trigger_handler(const struct device *dev,
			    const struct sensor_trigger *trigger)
{
	ARG_UNUSED(dev);
	ARG_UNUSED(trigger);

	k_sem_give(&sem);
}
#endif

int apds9960_init(void)
{
    apds.dev = DEVICE_DT_GET_ONE(avago_apds9960);
    if (!device_is_ready(apds.dev)) {
		LOG_ERR("sensor: device not ready.\n");
		return 0;
	}

#ifdef CONFIG_APDS9960_TRIGGER
	struct sensor_value attr = {
		.val1 = 64,
		.val2 = 0,
	};

	if (sensor_attr_set(apds.dev, SENSOR_CHAN_PROX,
			    SENSOR_ATTR_UPPER_THRESH, &attr)) {
		printk("Could not set threshold\n");
		return 1;
	}

	struct sensor_trigger trig = {
		.type = SENSOR_TRIG_THRESHOLD,
		.chan = SENSOR_CHAN_PROX,
	};

	if (sensor_trigger_set(apds.dev, &trig, trigger_handler)) {
		printk("Could not set trigger\n");
		return 1;
	}
#endif

    return 0;

}

/**
 * read sensor
 */
struct apds_data apds9960_read(void)
{
    struct apds_data data;

#ifdef CONFIG_APDS9960_TRIGGER
	k_sem_take(&sem, K_FOREVER);
#else
	//k_sleep(K_MSEC(250));
#endif

    if (sensor_sample_fetch(apds.dev)) {
        LOG_ERR("sensor_sample fetch failed\n");
    }

    sensor_channel_get(apds.dev, SENSOR_CHAN_LIGHT, &data.intensity);
	sensor_channel_get(apds.dev, SENSOR_CHAN_PROX, &data.prox);

    return data;
}