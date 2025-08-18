/**
 * @file main_app
 */
#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include "sys_app.h"
#include "msg_app.h"
#include "usb_comm.h"

#ifdef CONFIG_APDS9960
#include "apds9960.h"
struct apds_data sensdata;
#endif

LOG_MODULE_REGISTER(main_app, LOG_LEVEL_DBG);


/**
 * @brief main_app init process
 */
int main_app_init(void *arg)
{
    
    return 0;
}


/**
 * @brief main app run process
 */
int main_app_run(void *arg)
{
    static int count = 0;
    struct priv_data *data = (struct priv_data*) arg;
    struct message msg = {0};
    uint8_t temp[4] = {0};
    
    if (!data) {
        LOG_ERR("data not initted!\n");
        return 1;
    }

    usb_comm_process();
    
    if (count++ > 9) {
        count = 0;
        data->counter = (data->counter + 1) % 100;
        if (usb_comm_get_handshake_status()){
            usb_comm_send(FUNCTION_CODE_READ, ADDR_REG_COUNTER, &data->counter, 1);
        }
    }

#ifdef CONFIG_APDS9960
    sensdata = apds9960_read();
    LOG_DBG("Sensor int: %d prox: %d \n", sensdata.intensity.val1, sensdata.prox.val1);
    temp[0] = (uint8_t) sensdata.intensity.val1;
    temp[1] = (uint8_t) sensdata.prox.val1;
    LOG_DBG("sensor time: %d \n", sensdata.process_time);
    if (usb_comm_get_handshake_status()){
        usb_comm_send(FUNCTION_CODE_READ, ADDR_REG_SENSOR_APDS, temp, 2);
    }
#endif

    msg_app_read(&msg);
    switch( msg.id ) {
        case MSG_SET_LED:
            if (msg.data[0]) {
                gpio_pin_set_dt(data->led_red, 0);
            } else {
                gpio_pin_set_dt(data->led_red, 1);
            }
            break;
        default: break;
    }

    k_msleep(10);
    
    return 0;
}