#ifndef MSG_APP_H
#define MSG_APP_H

#include <stdint.h>

typedef enum 
{
    MSG_NONE				= 0x0000,	
	
	////Control the message set, and the key control messages in each mode///////////////////////////////////////
	MSG_MAIN_CLASS			= 0x1000, 	
	MSG_COMMON_CLOSE,				// The generic module runs out of the message, similar to WIN32 in WM_CLOSE
	MSG_STOP,
    MSG_VOL_UP,
    MSG_VOL_DW,
    MSG_FS_VOL_SET,
    MSG_PLAY_PAUSE,
    MSG_PLAY,
	MSG_PAUSE,
    MSG_FF_START,
    MSG_FB_START,
    MSG_FF_LP_START,
    MSG_FB_LP_START,
	MSG_FF_HOLD,
	MSG_FB_HOLD,
    MSG_FF_END,
	MSG_FB_END,
	MSG_FF_FB_END,
	MSG_PRE,
    MSG_NEXT,
    MSG_PRE_SEEK,
    MSG_NEXT_SEEK,
    MSG_REC,
    MSG_MUTE,
    MSG_POWER,  
    MSG_LANG,
    MSG_MODE,
	MSG_SET_MODE,
    MSG_SETUP,
	MSG_REC_FILE_DEL,	
	
	MSG_MENU,
	MSG_REVOLVE,
	MSG_BT_DIRECT,
	MSG_USB_DIRECT,
	MSG_LINEIN_DIRECT,
	MSG_FM_DIRECT,
	MSG_AUXIN_DIRECT,
	MSG_OPTICAL_DIRECT,

	MSG_SW_VERSION,

	MSG_EQ,
	MSG_SET_EQ,

	MSG_NAV_UP,
	MSG_NAV_UP_HOLD,
	MSG_NAV_DOWN,
	MSG_NAV_DOWN_HOLD,
	MSG_NAV_RIGHT,
	MSG_NAV_RIGHT_HOLD,
	MSG_NAV_LEFT,
	MSG_NAV_LEFT_HOLD,
	MSG_NAV_RIGHT_LP,
	MSG_NAV_LEFT_LP,	
	MSG_ENTER,

	MSG_RETURN,
	MSG_HOME,

	MSG_MEMORY,
	MSG_GOTO,
	MSG_CLEAR,
	
	MSG_NUM_0,	
	MSG_NUM_1,	
	MSG_NUM_2,	
	MSG_NUM_3,	
	MSG_NUM_4,	
	MSG_NUM_5,	
	MSG_NUM_6,	
	MSG_NUM_7,	
	MSG_NUM_8,	
	MSG_NUM_9,	

	MSG_LIGHT,
	MSG_LIGHT_SET,
	MSG_LIGHT_RESET,	
	MSG_LIGHT_UP,
	MSG_LIGHT_DW,

	MSG_LIGHT_MUSIC,
	MSG_LIGHT_RANDOM,

	MSG_EXT_EQ,
	MSG_EXT_EQ_SET,
	MSG_EXT_EQ_RESET,
	MSG_EXT_EQ_UP,
	MSG_EXT_EQ_DW,

	// RTC
	MSG_RTC_SET_TIME,
	MSG_RTC_SET_ALARM,
	MSG_RTC_SET_SLEEP,
	MSG_RTC_SET_TIMER_ON,
	MSG_RTC_SET_TIMER_OFF,

	// play control(Non-key generated messages)
	MSG_NEXT_SONG,		
	MSG_PRE_SONG,

	MSG_FACTORY_RESET,

	MSG_SET_LED,

	MSG_BT_BROADCAST,

	MSG_SPEAKER_MODE,
	
	/** volume for bluetooth broadcast sync*/
	MSG_BT_BROADCAST_SYNC_VOL,

} MessageId;


struct message
{
	uint16_t id;
	uint8_t data[4];
	uint8_t len;
};

struct message * msg_app_get_ctx(void);
void msg_app_send(uint16_t id, uint8_t *data, uint8_t len);
int msg_app_read(struct message *msg);

#endif
