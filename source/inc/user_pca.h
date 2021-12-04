/*
 * user_pca.h
 *
 *  Created on: 2019年9月27日
 *      Author: Bough_004
 */

#ifndef SOURCE_INC_USER_PCA_H_
#define SOURCE_INC_USER_PCA_H_
#include "const.h"
#include "pca.h"

extern	int32_t userPcaConfig(void);
extern	void user_Pca_CapData16Set(en_pca_module_t enModule, uint16_t u16Data);
extern	void updateTo16MPCA(void);
extern	void updateTo24MPCA(void);
void PcaInt(void);
#endif /* SOURCE_INC_USER_PCA_H_ */
