/*
 * user_lptimer.h
 *
 *  Created on: 2019年9月27日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_LPTIMER_H_
#define SOURCE_INC_USER_LPTIMER_H_

#include "gpio.h"
#include "io_define.h"
#include "lpt.h"

extern	u16 vSystemLIRC5ms;
extern	u16 vSystem5ms;
extern	u8	vTimerCnt;
extern	u8	fTime45ms;
extern	u8	fTime50ms;
extern	u8	fTime100ms;
extern	u8	fTime500ms;
extern	u8	fTime1s;
extern	u16 	vSecondCnt;
//extern	void LPTIMER_Config(void);
void 	LPTIMER_Config(void);
//void 	Enter_DeepSleep(u8 clkDIV,u8 awakeValue);
void 	Enter_DeepSleep(void);
void 	Enter_Sleep(void);

void 	LptInt(void);
#endif /* SOURCE_INC_USER_LPTIMER_H_ */
