/*
 * user_exti.h
 *
 *  Created on: 2019年9月28日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_EXTI_H_
#define SOURCE_INC_USER_EXTI_H_
#include "const.h"
extern	u16	vClosedKeyTimer;
extern	u16	vFuncKeyTimer;

//引脚定义





/* define------------------------*/
void EXTI_Key_Config(void);
void enableDcDectISR(void);
void disableDcDectISR(void);
//void setClosedKeyRising(void);
//void setClosedKeyFalling(void);

#endif /* SOURCE_INC_USER_EXTI_H_ */
