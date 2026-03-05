/*
 * ap.c
 *
 *  Created on: Aug 6, 2025
 *      Author: user
 */


#include "ap.h"
// #include "bsp.h"

// uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

void apInit(void)
{
  uartOpen(_DEF_UART1, 57600); // 1번채널은 USB
  uartOpen(_DEF_UART2, 57600); // 2번채널은 실제 물리적인 UART
}


void apMain(void)
{
    uint32_t pre_time;

    pre_time = millis();
    while(1)
    {
      if (millis()-pre_time >= 500)
      {
        pre_time = millis();
        ledToggle(_DEF_LED1);
      }
   }
}
// main.c를 최소화하고 ap 상위작업은 모두 여기서 할수 있도록
