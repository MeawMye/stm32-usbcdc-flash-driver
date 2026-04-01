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

// main.c를 최소화하고 ap 상위작업은 모두 여기서 할수 있도록
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

      FlashCommandTest(_DEF_UART1);
    }
}

void FlashCommandTest(uint8_t uart_ch)
{
    if (uartAvailable(uart_ch) > 0)
      {
        uint8_t rx_data = uartRead(uart_ch);

        switch(rx_data)
        {
          case '1':
          {
            uint8_t buf[32];

            logPrintf("Read...\n");

             __HAL_FLASH_DATA_CACHE_DISABLE();
             __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
             __HAL_FLASH_DATA_CACHE_RESET();
             __HAL_FLASH_INSTRUCTION_CACHE_RESET();
             __HAL_FLASH_DATA_CACHE_ENABLE();
             __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();

            flashRead(0x08010000, buf, 32);

            for (int i=0; i<32; i++)
            {
              logPrintf("0x%X : 0x%X\n", 0x08010000 + i, buf[i]);
            }
          }

          break;

          case '2':
          {
            logPrintf("Erase...\n");
            if (flashErase(0x08010000, 32) == true)
              logPrintf("Erase OK\n");
            else
              logPrintf("Erase Fail\n");
          }
          break;

          // Write전에 Erase를 우선 실행해야힘.
          case '3':
          {
            uint8_t buf[32];

            for (int i=0; i<32; i++)
            {
             buf[i] = i;
           }

            logPrintf("Write...\n");

            if (flashWrite(0x08010000, buf, 32) == true)
            {
              logPrintf("Write OK\n");
            }
            else
            {
              logPrintf("Write Fail\n");
            }

          }
          break;
        }
      }
}
