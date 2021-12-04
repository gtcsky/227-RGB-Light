/*
 * user_i2c.h
 *
 *  Created on: 2019年10月16日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_I2C_H_
#define SOURCE_INC_USER_I2C_H_
#include "const.h"
#include "gpio.h"
#include "i2c.h"
#define 	I2C_DEVADDR 	0xEA
#define	I2C_SLAVEADDR	0xEA
#define 	READLEN   10


#define	SYSTEM_STATUS_ADDR	0xD1
#define	CHARGE_STATUS_ADDR	0xD7
#define	VIN_VBUS_STATE			0xD5

#define	CHG_CTL2				0x22
#define	CHG_CTL3				0x23
#define	SYS_CTL4				0x04
#define	IC_TEMP					0x42
#define	NTC_FLAG				0xDA
#define	FLAG0					0x7E
#define	RSET					0x82
#define	SYS_CTL7				0x07
#define	CHG_TIMER_EN			0x2C
#define	SYS_CTL3				0x03
#define	SYS_CTL14				0x0E
#define	BOOST_LINC				0x13
#define	VBAT_LOW				0x10
#define	VBUSOV					0x12
#define	PMOS_REG_CTL1			0x5A
#define	QC_EN					0x3E
#define	DCP_DIG_CTL2			0xA2
#define	QC_VMAX				0x96

extern	en_result_t userI2CInit(void);
extern	en_result_t	 readVolt(void);
extern	en_result_t I2C_MasterReadData(en_i2c_channel_t enCh,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len);
extern	en_result_t I2C_MasterWriteData(en_i2c_channel_t enCh,uint8_t u8Addr,uint8_t *pu8Data,uint32_t u32Len);
#endif /* SOURCE_INC_USER_I2C_H_ */
