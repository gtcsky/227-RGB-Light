/*
 * user_adc.h
 *
 *  Created on: 2019年9月28日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_ADC_H_
#define SOURCE_INC_USER_ADC_H_
#include "const.h"
#include "adc.h"

#define	BATT_CONVERTING	0X01			//正在转换电池电压
#define	TEMP_CONVERTING	0X02			//正在转换温度
#define	VOL1V2_CONVERTING	0X04		//正在转换内部1.2v基准电压

extern	float vBattVol;						//有MCU的ADC读出的电池电压
extern	float vAdVoltOfBatt;
extern	float vTestCompBatt;
extern	float battArray[BATT_ARRAY_SIZE];
extern	volatile  uint32_t vSystemTemperature;
extern	u16 getMutilChlsADC(u32 channels);
extern	u8 adcInterVrefEnable(void);
extern	u16 getTemperatureADC(void);
extern	u8 getSingleChannelADC(en_adc_samp_ch_sel_t channel);
#endif /* SOURCE_INC_USER_ADC_H_ */
