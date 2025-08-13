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

			//LOG_DBG("tty fifo -> ringbuf %d bytes", rb_len);
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
	// uart_irq_tx_enable(uart_dev);

    return 0;
}

static uint16_t crc16_ccitt(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFF;
    uint16_t polynomial = 0xA001;

    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)data[i];

        for (size_t j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ polynomial;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

/**
 * send modbus rtu formatted protocol
 */
int usb_comm_send(
	uint16_t fc_code, 
	uint16_t reg, 
	uint8_t *data, 
	uint8_t len)
{
	uint8_t buffer[64];
	uint16_t checksum;

	/** send header first */
	buffer[0] = COM_HEADER_1ST;
	buffer[1] = COM_HEADER_2ND;
	buffer[2] = COM_HEADER_3RD;
	buffer[3] = COM_HEADER_4TH;

	buffer[4] = fc_code;
	buffer[5] = len;
	buffer[6] = (reg >> 8) & 0xff;
	buffer[7] = reg & 0xff;

	if (len > 0) {
		for (int i = 0; i < len; i++) {
			buffer[8 + i] = data[i];
		}
	}

	checksum = crc16_ccitt(buffer, len + 8);

	// set checksum word
	buffer[8 + len] = (checksum>>8) & 0xff;
	buffer[9 + len] = checksum & 0xff;

	//for (int i = 0; i < (len + 9); i += 1) {
		uart_tx(uart_dev, buffer, (len + 9), 100);
	//}
	// uart_fifo_fill(uart_dev, buffer, 9 + len);
}

static int process_data(uint16_t fc_code, uint16_t reg, uint8_t *data)
{
	switch (fc_code) {
		default: break;
		case FC_CODE_READ:

			break;
		case FC_CODE_WRITE:

			break;
	}
}

#define	DATA_COMPLETE_OFFSET(len)	(9 + len)
#define	FC_CODE_OFFSET		4

void usb_comm_process(void)
{
	int rx_index = 0;
	int len = 0;
	uint8_t buffer[64] = {0};
	uint16_t cmd;
	uint16_t cs;
	uint8_t func_code;

	while (ring_buf_size_get(&ringbuf) > 0)
	{
		char temp = 0;
		ring_buf_get(&ringbuf, &temp, 1);

		buffer[rx_index] = temp;
		/** check header */
		if (rx_index == 0) {
			if (temp == COM_HEADER_1ST) rx_index = 1; else rx_index = 0; 
		} else if (rx_index == 1) {
			if (temp == COM_HEADER_2ND) rx_index = 2; else rx_index = 0;
		} else if (rx_index == 2) {
			if (temp == COM_HEADER_3RD) rx_index = 3; else rx_index = 0;
		} else if (rx_index == 3) {
			if (temp == COM_HEADER_4TH) rx_index = 4; else rx_index = 0;
		} else {
			
			/** get data len and command/reg */
			if (rx_index == 7) {
				func_code = buffer[0];
				len = buffer[5];
				cmd = (buffer[6] << 8) | (buffer[7] & 0xff);

			} else if (rx_index >= (DATA_COMPLETE_OFFSET(len)) ) {
				cs = (buffer[DATA_COMPLETE_OFFSET(len)-1] << 8) | buffer[DATA_COMPLETE_OFFSET(len)];
				printk("[%04X] [%04X]\n", cs, crc16_ccitt(buffer, (DATA_COMPLETE_OFFSET(len)-1)));
				if (cs == crc16_ccitt(buffer, (DATA_COMPLETE_OFFSET(len)-1))){
					printk("CS OK\n");
				}
				//printk("CS: %04X", cs);
				// set global data buffer from buffer
				/*printk("Do process data: ");
				for(int i = 0; i < len; i++) {
					printk("%d ", buffer[8 + i]);
				}
				printk("Done!\n");*/
				process_data(func_code, cmd, buffer[8]);
				rx_index = 0;	// reset rx index
			}
			rx_index++;
		}
	}
}
