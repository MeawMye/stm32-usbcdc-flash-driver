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
#define _USE_HW_CDC
#define _USE_HW_FLASH
// 2MB 기준 Sector 11(128KB, 뱅크1 마지막). 펌웨어(0x08005000~)보다 한참 뒤라 겹치지 않는 테스트 전용 영역
#define       HW_FLASH_TEST_ADDR  0x080E0000
#define       HW_FLASH_TEST_LEN   32

#define _USE_HW_LED
#define       HW_LED_MAX_CH       1

#define _USE_HW_UART
#define       HW_UART_MAX_CH      2




#endif /* SRC_HW_HW_DEF_H_ */
