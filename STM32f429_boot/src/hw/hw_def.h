/*
 * hw_def.h
 *
 *  Created on: Aug 6, 2025
 *      Author: user
 */

#ifndef SRC_HW_HW_DEF_H_
#define SRC_HW_HW_DEF_H_


// 하드웨어 모듈들이 공통으로 사용할 정의들, hw.c와 hw.h는 드라이버 모듈이라 칭함
#include "def.h"
#include "bsp.h"


#define _USE_HW_USB
#define _USE_HW_RTC
#define _USE_HW_RESET
//#define _USE_HW_CDC
#define _USE_HW_FLASH

#define _USE_HW_LED
#define       HW_LED_MAX_CH       1

/* #define _USE_HW_UART
#define       HW_UART_MAX_CH      2 */




#endif /* SRC_HW_HW_DEF_H_ */
