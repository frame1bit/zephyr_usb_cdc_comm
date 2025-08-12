#ifndef USB_COMM_H
#define USB_COMM_H

#define COM_HEADER_1ST  0x12
#define COM_HEADER_2ND  0x21
#define COM_HEADER_3RD  0x34
#define COM_HEADER_4TH  0x76

/* function prototype */
int usb_comm_init(void);
void usb_comm_process(void);
/* end of function prototype */

#endif
