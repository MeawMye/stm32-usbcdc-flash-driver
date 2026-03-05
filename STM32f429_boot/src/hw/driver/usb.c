/*
 * usb.c
 *
 *  Created on: Aug 14, 2025
 *      Author: user
 */


#include  "usb.h"
#include "usb_device.h"




bool usbInit(void)
{
  bool ret = true;



  GPIO_InitTypeDef GPIO_InitStruct = {0};

   GPIO_InitStruct.Pin = GPIO_PIN_12;
   GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
   GPIO_InitStruct.Pull = GPIO_NOPULL;
   GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_RESET);
   delay(200);
   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, GPIO_PIN_SET);

   GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
   HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // USB가 보드자체 RESET 시 리로드 되도록 설정한 코드

   MX_USB_DEVICE_Init();

   return ret;
}
