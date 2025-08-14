#ifndef USB_COMM_H
#define USB_COMM_H

#define COM_HEADER_1ST  0x12
#define COM_HEADER_2ND  0x21
#define COM_HEADER_3RD  0x34
#define COM_HEADER_4TH  0x76

#define FUNCTION_CODE_WRITE 0x06
#define FUNCTION_CODE_READ  0x03

// read
#define ADDR_REG_HANDSHAKE  0x0000
#define ADDR_REG_SET_LED    0x0001

/* function prototype */
int usb_comm_init(void);
void usb_comm_process(void);
int usb_comm_send(uint16_t fc_code, uint16_t reg, uint8_t *data, uint8_t len);
/* end of function prototype */

#endif
