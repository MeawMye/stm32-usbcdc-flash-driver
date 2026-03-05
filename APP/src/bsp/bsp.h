/*
 * bsp.h
 *
 *  Created on: Aug 6, 2025
 *      Author: user
 */

#ifndef SRC_BSP_BSP_H_
#define SRC_BSP_BSP_H_

#include "def.h" // common 내 공통으로 사용하는 파일


#define _USE_LOG_PRINT    1

#if _USE_LOG_PRINT
#define logPrintf(fmt, ...)     printf(fmt, ##__VA_ARGS__)
#else
#define logPrintf(fmt, ...)
#endif


#include "stm32f4xx_hal_conf.h"



void bspInit(void);


// 여기 적용되는 것들은 API
// 모든 부분에서 공통으로 사용하는 함수를 여기에 정의할 것
void delay(uint32_t ms); // ms 단위의 입력값을 받아서 delay
uint32_t millis(void);


void SystemClock_Config(void);
void Error_Handler(void);



#endif /* SRC_BSP_BSP_H_ */
