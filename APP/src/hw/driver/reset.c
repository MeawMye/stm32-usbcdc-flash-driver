/*
 * reset.c
 *
 *  Created on: Aug 16, 2025
 *      Author: y6484
 */


#include "reset.h"
#include "rtc.h"


static uint32_t reset_count = 0;


bool resetInit(void)
{
  bool ret = true;

  // 만약 리셋핀이 눌렸다면
  if (RCC->CSR & (1<<26))
  {
    rtcBackupRegWrite(0, rtcBackupRegRead(0) + 1);
    delay(500);
    reset_count = rtcBackupRegRead(0);
  }

  rtcBackupRegWrite(0, 0);


  return ret;
}

uint32_t resetGetCount(void)
{
  return reset_count;
}

uint32_t resetGetCause(void)
{
  return RCC->CSR & (RCC_CSR_LPWRRSTF |
                     RCC_CSR_WWDGRSTF |
                     RCC_CSR_IWDGRSTF |
                     RCC_CSR_SFTRSTF |
                     RCC_CSR_PORRSTF |
                     RCC_CSR_PINRSTF |
                     RCC_CSR_BORRSTF);
}

void resetClearCause(void)
{
  __HAL_RCC_CLEAR_RESET_FLAGS();
}
