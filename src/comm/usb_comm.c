#include <sample_usbd.h>

#include <zephyr/device.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/ring_buffer.h>
#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/logging/log.h>
#include "usb_comm.h"

LOG_MODULE_REGISTER(usb_comm, LOG_LEVEL_DBG);


const struct device *const uart_dev = DEVICE_DT_GET_ONE(zephyr_cdc_acm_uart);

#define RING_BUF_SIZE 1024
uint8_t ring_buffer[RING_BUF_SIZE];

struct ring_buf ringbuf;

static inline void print_baudrate(const struct device *dev)
{
	uint32_t baudrate;
	int ret;

	ret = uart_line_ctrl_get(dev, UART_LINE_CTRL_BAUD_RATE, &baudrate);
	if (ret) {
		LOG_WRN("Failed to get baudrate, ret code %d", ret);
	} else {
		LOG_INF("Baudrate %u", baudrate);
	}
}


static void interrupt_handler(const struct device *dev, void *user_data)
{
	ARG_UNUSED(user_data);

	while (uart_irq_update(dev) && uart_irq_is_pending(dev)) {
		if (uart_irq_rx_ready(dev)) {
			int recv_len, rb_len;
			uint8_t buffer[64];
			size_t len = MIN(ring_buf_space_get(&ringbuf),
					 sizeof(buffer));

			if (len == 0) {
				/* Throttle because ring buffer is full */
				uart_irq_rx_disable(dev);
				continue;
			}

			recv_len = uart_fifo_read(dev, buffer, len);
			if (recv_len < 0) {
				LOG_ERR("Failed to read UART FIFO");
				recv_len = 0;
			};

			rb_len = ring_buf_put(&ringbuf, buffer, recv_len);
			if (rb_len < recv_len) {
				LOG_ERR("Drop %u bytes", recv_len - rb_len);
			}

			LOG_DBG("tty fifo -> ringbuf %d bytes", rb_len);
		}
	}
}

int usb_comm_init(void)
{
    int ret;

	if (!device_is_ready(uart_dev)) {
		LOG_ERR("CDC ACM device not ready");
		return 1;
	}

	ret = usb_enable(NULL);
    if (ret != 0) {
		LOG_ERR("Failed to enable USB");
		return 1;
	}

    ring_buf_init(&ringbuf, sizeof(ring_buffer), ring_buffer);

    /* Wait 100ms for the host to do all settings */
	k_msleep(100);

    uart_irq_callback_set(uart_dev, interrupt_handler);

	/* Enable rx interrupts */
	uart_irq_rx_enable(uart_dev);

    return 0;
}


void usb_comm_process(void)
{
    size_t avail = ring_buf_size_get(&ringbuf);
    if (avail > 0) {
        uint8_t buffer[64];
        int rb_len;

        rb_len = ring_buf_get(&ringbuf, buffer, sizeof(buffer));
        if (!rb_len) {
            //LOG_DBG("Ring buffer empty, disable TX IRQ");
        }

        for(int i = 0; i < rb_len; i++) {
            printk("%c ", buffer[i]);
        }
    }
}
