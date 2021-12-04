/*
 * user_color.h
 *
 *  Created on: 2019��9��26��
 *      Author: Bough_004
 */

#ifndef USER_INC_USER_COLOR_H_
#define USER_INC_USER_COLOR_H_

#include "const.h"
#include "io_define.h"
extern	void hsb2Rgb(colorStructType * colorStruct);
extern	void updateColor(colorStructType * colorStruct);
extern	void updateColorTemp(displayParamsStruct * disParams);
extern	void	setTargeColorTempData(displayParamsStruct * disParams,PcaDataStruct *targetData);
extern	void turnOffRGBLamp(void);
extern	void turnOffColorTempLamp(void);
extern	void superFlashMode(u8 index);
extern	void updateBrightnessByTemperature(void);
#endif /* USER_INC_USER_COLOR_H_ */
