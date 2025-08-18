#include "msg_app.h"

struct message main_msg;

struct message * msg_app_get_ctx(void)
{
    return &main_msg;
}


void msg_app_send(uint16_t id, uint8_t *data, uint8_t len)
{
    if (len > sizeof(main_msg.data))
        return;
    main_msg.len = 0;
    main_msg.id = id;
    while(len--) {
        main_msg.data[main_msg.len] = data[main_msg.len];
        main_msg.len++;
        //printk("send: len %d \n", main_msg.len);
    }
}

int msg_app_read(struct message *msg)
{
    msg->id = main_msg.id;
    for(int i=0; i < main_msg.len; i++) {
        msg->data[i] = main_msg.data[i]; 
        //printk("data: %d \n", main_msg.data[i]);
    }

    main_msg.id = MSG_NONE;
    main_msg.len = 0;
    
    return 0;
}