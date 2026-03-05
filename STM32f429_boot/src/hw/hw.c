/*
 * hw.c
 *
 *  Created on: Aug 6, 2025
 *      Author: user
 */


#include "hw.h"



void hwInit(void)
{
  bspInit();
  rtcInit();

  resetInit();
  ledInit();
  usbInit();
//  uartInit();
}
