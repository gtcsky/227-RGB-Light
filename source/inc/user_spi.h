/*
 * user_spi.h
 *
 *  Created on: 2019年10月10日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_SPI_H_
#define SOURCE_INC_USER_SPI_H_

#include "const.h"
#include "spi.h"
#include	"io_define.h"

extern	void WriteData(en_spi_channel_t enCh,uint8_t *sendstr,uint8_t sendlen);
extern	void ReadData(en_spi_channel_t enCh,uint8_t *recvstr,uint8_t recvlen);
extern	void userSpiInit(void);
#endif /* SOURCE_INC_USER_SPI_H_ */
