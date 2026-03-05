/*
 * flash.h
 *
 *  Created on: Aug 21, 2025
 *      Author: user
 */

#ifndef SRC_COMMON_HW_INCLUDE_FLASH_H_
#define SRC_COMMON_HW_INCLUDE_FLASH_H_

#include "hw_def.h"

#ifdef _USE_HW_FLASH


//bool flashInit(void);
bool flashErase(uint32_t base, uint32_t size);
bool flashWrite(uint32_t base, uint8_t *p_data, uint32_t size);
bool flashRead(uint32_t base, uint8_t *p_data, uint32_t size);


#endif

#endif /* SRC_COMMON_HW_INCLUDE_FLASH_H_ */
