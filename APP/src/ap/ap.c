/*
 * ap.c
 *
 *  Created on: Aug 6, 2025
 *      Author: user
 */


#include "ap.h"
// #include "bsp.h"

// uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

#define FLASH_TEST_ADDR   HW_FLASH_TEST_ADDR
#define FLASH_TEST_LEN    HW_FLASH_TEST_LEN

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
  uart_bridge_enabled = !uart_bridge_enabled;
  logPrintf("USER button (PC13) pressed\n");
  logPrintf("UART bridge: %s\n", uart_bridge_enabled ? "ON" : "OFF");
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
  static uint8_t to_uart[32];
  static uint8_t to_cdc[32];  // USB 비동기 전송 중 덮어쓰지 않도록 static
  uint32_t length = 0;

  while (length < sizeof(to_uart) && uartAvailable(_DEF_UART1) > 0)
  {
    to_uart[length++] = uartRead(_DEF_UART1);
  }

  if (length > 0)
  {
    uartWrite(_DEF_UART2, to_uart, length);
  }

  // CDC 송신이 끝난 경우에만 읽어서, 전송 실패로 데이터가 유실되지 않게 함
  if (uartTxBusy(_DEF_UART1))
  {
    return;
  }

  length = 0;
  while (length < sizeof(to_cdc) && uartAvailable(_DEF_UART2) > 0)
  {
    to_cdc[length++] = uartRead(_DEF_UART2);
  }

  if (length > 0)
  {
    uartWrite(_DEF_UART1, to_cdc, length);
  }
}

void apInit(void)
{
  uartOpen(_DEF_UART1, 115200); // USB CDC
  uartOpen(_DEF_UART2, 115200); // USART6 (PG14 TX, PG9 RX)
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

            logPrintf("You push the button 1. Read test.\n");
            logPrintf("Read... addr: 0x%lX, len: %d\n", (unsigned long)FLASH_TEST_ADDR, FLASH_TEST_LEN);

             __HAL_FLASH_DATA_CACHE_DISABLE();
             __HAL_FLASH_INSTRUCTION_CACHE_DISABLE();
             __HAL_FLASH_DATA_CACHE_RESET();
             __HAL_FLASH_INSTRUCTION_CACHE_RESET();
             __HAL_FLASH_DATA_CACHE_ENABLE();
             __HAL_FLASH_INSTRUCTION_CACHE_ENABLE();

            flashRead(FLASH_TEST_ADDR, buf, FLASH_TEST_LEN);

            for (int i=0; i<FLASH_TEST_LEN; i++)
            {
              logPrintf("0x%X : 0x%X\n", FLASH_TEST_ADDR + i, buf[i]);
            }
          }

          break;

          case '2':
          {
            logPrintf("Erase... addr: 0x%lX, len: %d\n", (unsigned long)FLASH_TEST_ADDR, FLASH_TEST_LEN);
            if (flashErase(FLASH_TEST_ADDR, FLASH_TEST_LEN) == true)
            {
              logPrintf("You push the button 2. Erase test.\n");
              logPrintf("When you write, make sure to erase first.\n");
              logPrintf("Erase OK\n");
            }
            else
              logPrintf("Erase Fail\n");
          }
          break;

          // Write전에 Erase를 우선 실행해야힘.
          case '3':
          {
            uint8_t buf[FLASH_TEST_LEN];

            for (int i=0; i<FLASH_TEST_LEN; i++)
            {
             buf[i] = i;
           }
            logPrintf("You push the button 3. Write test.\n");
            logPrintf("When you write, make sure to erase first.\n");
            logPrintf("Write... addr: 0x%lX, len: %d\n", (unsigned long)FLASH_TEST_ADDR, FLASH_TEST_LEN);

            if (flashWrite(FLASH_TEST_ADDR, buf, FLASH_TEST_LEN) == true)
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
