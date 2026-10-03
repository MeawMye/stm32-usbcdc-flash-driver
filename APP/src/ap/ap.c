/*
 * ap.c
 *
 *  Created on: Aug 6, 2025
 *      Author: user
 */


#include "ap.h"
// #include "bsp.h"

// uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

static GPIO_PinState button_raw_state;
static GPIO_PinState button_stable_state;
static uint32_t button_change_time;
static bool uart_bridge_enabled;

static void apLedTask(uint32_t *pre_time)
{
  uint32_t now = millis();

  if (now - *pre_time >= 500)
  {
    *pre_time = now;
    ledToggle(_DEF_LED1);
  }
}

static void apButtonPressed(void)
{
  uint32_t cause = resetGetCause();

  logPrintf("Reset count: %lu\n", (unsigned long)resetGetCount());
  if (cause == 0)
  {
    logPrintf("No reset cause flags set\n");
  }
  if (cause & RCC_CSR_PINRSTF)  logPrintf("Reset cause: NRST pin\n");
  if (cause & RCC_CSR_PORRSTF)  logPrintf("Reset cause: power-on/power-down\n");
  if (cause & RCC_CSR_BORRSTF)  logPrintf("Reset cause: brown-out\n");
  if (cause & RCC_CSR_SFTRSTF)  logPrintf("Reset cause: software\n");
  if (cause & RCC_CSR_IWDGRSTF) logPrintf("Reset cause: independent watchdog\n");
  if (cause & RCC_CSR_WWDGRSTF) logPrintf("Reset cause: window watchdog\n");
  if (cause & RCC_CSR_LPWRRSTF) logPrintf("Reset cause: low-power\n");

  uart_bridge_enabled = !uart_bridge_enabled;
  logPrintf("UART bridge: %s\n", uart_bridge_enabled ? "ON" : "OFF");

  resetClearCause();
}

static void apButtonInit(void)
{
  GPIO_InitTypeDef gpio_init = {0};

  __HAL_RCC_GPIOC_CLK_ENABLE();
  gpio_init.Pin = GPIO_PIN_13;
  gpio_init.Mode = GPIO_MODE_INPUT;
  gpio_init.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOC, &gpio_init);

  button_stable_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
  button_raw_state = button_stable_state;
  button_change_time = millis();
}

static void apButtonTask(void)
{
  uint32_t now = millis();
  GPIO_PinState raw_state = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

  if (raw_state != button_raw_state)
  {
    button_raw_state = raw_state;
    button_change_time = now;
  }

  if (button_raw_state != button_stable_state && now - button_change_time >= 30)
  {
    button_stable_state = button_raw_state;
    if (button_stable_state == GPIO_PIN_SET)
    {
      apButtonPressed();
    }
  }
}

static void apUsbCommandTask(void)
{
  FlashCommandTest(_DEF_UART1);
}

static void apUartBridgeTask(void)
{
  uint8_t data[32];
  uint32_t length = 0;

  while (length < sizeof(data) && uartAvailable(_DEF_UART1) > 0)
  {
    data[length++] = uartRead(_DEF_UART1);
  }

  if (length > 0)
  {
    uartWrite(_DEF_UART2, data, length);
  }

  length = 0;
  while (length < sizeof(data) && uartAvailable(_DEF_UART2) > 0)
  {
    data[length++] = uartRead(_DEF_UART2);
  }

  if (length > 0)
  {
    uartWrite(_DEF_UART1, data, length);
  }
}

void apInit(void)
{
  uartOpen(_DEF_UART1, 57600); // USB CDC
  uartOpen(_DEF_UART2, 57600); // USART1
  apButtonInit();
}

// main.c를 최소화하고 ap 상위작업은 모두 여기서 할수 있도록
void apMain(void)
{
    uint32_t pre_time = millis();

    while(1)
    {
      apLedTask(&pre_time);
      apButtonTask();
      if (uart_bridge_enabled)
      {
        apUartBridgeTask();
      }
      else
      {
        apUsbCommandTask();
      }
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
