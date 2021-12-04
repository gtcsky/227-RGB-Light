/*
 * user_key.h
 *
 *  Created on: 2019年8月15日
 *      Author: Bough_004
 */

#ifndef USER_INC_USER_KEY_H_
#define USER_INC_USER_KEY_H_
#include "const.h"
//#include "user_exti.h"

#define FUNC_KEY_SHORT_PRESSED    	0x51
#define FUNC_KEY_LONG_PRESSED    	0x52
#define FUNC_KEY_DOUBLE_CLICK    	0x53

#define POWER_KEY_SHORT_PRESSED    	0x01
#define POWER_KEY_LONG_PRESSED    	0x02

#define INC_KEY_SHORT_PRESSED    	0x11
#define INC_KEY_LONG_PRESSED    		0x12

#define DEC_KEY_SHORT_PRESSED    	0x21
#define DEC_KEY_LONG_PRESSED    		0x22

#define BRIGHTNESS_UP_KEY_SHORT_PRESSED    			0x31
#define BRIGHTNESS_UP_KEY_LONG_PRESSED    			0x32

#define BRIGHTNESS_DOWN_KEY_SHORT_PRESSED    		0x41
#define BRIGHTNESS_DOWN_KEY_LONG_PRESSED    		0x42

#define CLOSE_KEY_SHORT_PRESSED   	 0x61
#define CLOSE_KEY_LONG_PRESSED    	 0x62

#define SHORT_PRESSED_TIME   	2  		//4*5ms=20ms
#define LONG_PRESSED_0S3     		60		//60*5=300ms
#define LONG_PRESSED_0S4     		80		//80*5=400ms
#define LONG_PRESSED_0S5     		100		//100*5=500ms
#define LONG_PRESSED_1S			200		//200*5ms=1s
#define LONG_PRESSED_2S			400		//400*5ms=2s
#define LONG_PRESSED_3S			600		//600*5ms=3s
#define LONG_PRESSED_4S			800		//800*5ms=4s
#define LONG_PRESSED_5S			1000	//1000*5ms=5s

#define DOUBLE_CLICK_CONST					100;   		//100*5ms  在第一次按键释放候,倒计时500ms以内,如果右第二次按键产生,就当成是双击
extern	u8	vKeyValue;
extern	u8 	vIsKeyPressed;
extern	u8 	vIsFuncKeyGod;
extern	u8	vDoubleClickCountDown;
extern	u16	vFuncKeyTimer;

extern	u8 	vIsPowerKeyGod;
extern	u16	vPowerKeyTimer;

extern	u8 	vIsIncKeyGod;
extern	u16	vIncKeyTimer;

extern	u16	vDecKeyTimer;
extern	u8 	vIsDecKeyGod;

extern	u16	vBrightnessUpKeyTimer;
extern	u8 	vIsBrightnessUpKeyGod;

extern	u16	vBrightnessDownKeyTimer;
extern	u8 	vIsBrightnessDownKeyGod;

#define	FUNC_KEY_SHOT_PRRESS_GOD	0X01
#define	FUNC_KEY_LONG_PRRESS_GOD	0X02
#define	POWER_KEY_SHOT_PRRESS_GOD	0X01
#define	POWER_KEY_LONG_PRRESS_GOD	0X02

#define	INC_KEY_SHOT_PRRESS_GOD	0X01
#define	INC_KEY_LONG_PRRESS_GOD	0X02

#define	DEC_KEY_SHOT_PRRESS_GOD	0X01
#define	DEC_KEY_LONG_PRRESS_GOD	0X02

#define	BRIGHTNESS_UP_KEY_SHOT_PRRESS_GOD	0X01
#define	BRIGHTNESS_UP_KEY_LONG_PRRESS_GOD	0X02

#define	BRIGHTNESS_DOWN_KEY_SHOT_PRRESS_GOD	0X01
#define	BRIGHTNESS_DOWN_KEY_LONG_PRRESS_GOD	0X02

#define	CLOSED_KEY_SHOT_PRRESS_GOD	0X01
#define	CLOSED_KEY_LONG_PRRESS_GOD	0X02


extern	void funcKeyTask(void);
extern	void keyDoubleClickChk(void);
extern	void setClosedKeyPressedFlag(void);
extern	void setFuncKeyPressedFlag(void);
extern	void setPowerKeyPressedFlag(void);
extern	void closedReleasedFunc(void);
extern	void setIncKeyPressedFlag(void);
extern	void setDecKeyPressedFlag(void);
extern	void holdingKeyFunc(void);
extern	void setBrightnessUpKeyPressedFlag(void);
extern	void setBrightnessDownKeyPressedFlag(void);

#endif /* USER_INC_USER_KEY_H_ */
