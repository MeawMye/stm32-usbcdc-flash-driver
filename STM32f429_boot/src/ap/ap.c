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
}


void apMain(void)
{
    uint32_t pre_time;


    pre_time = millis();
    while(1)
    {
      if (millis()-pre_time >= 100)
      {
        pre_time = millis();
        ledToggle(_DEF_LED1);
      }
   }
}
// main.c를 최소화하고 ap 상위작업은 모두 여기서 할수 있도록
