/*
 * uart.c
 *
 *  Created on: Aug 13, 2025
 *      Author: y6484
 */


#include "uart.h"
#include "cdc.h"
#include "qbuffer.h"


static bool is_open[UART_MAX_CH];


static qbuffer_t qbuffer[UART_MAX_CH];
static uint8_t rx_buf[256];


UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;


bool uartInit(void)
{
  for (int i=0; i<UART_MAX_CH; i++)
  {
    is_open[i] = false;
  }

  return true;
}

bool uartOpen(uint8_t ch, uint32_t baud)
{
  bool ret = false; //리턴이 bool이니까 동일하게 bool로 하고 성공했을때만 true되게


  switch(ch)  //혹시 모르니까 여러 채널에 대응할 수 있게 설계
  {
    case _DEF_UART1:
      is_open[ch] = true;
      ret = true;
      break;
    case _DEF_UART2:
      huart1.Instance           = USART1;
      huart1.Init.BaudRate      = baud;
      huart1.Init.WordLength    = UART_WORDLENGTH_8B;
      huart1.Init.StopBits      = UART_STOPBITS_1;
      huart1.Init.Parity        = UART_PARITY_NONE;
      huart1.Init.Mode          = UART_MODE_TX_RX;
      huart1.Init.HwFlowCtl     = UART_HWCONTROL_NONE;
      huart1.Init.OverSampling  = UART_OVERSAMPLING_16;


      HAL_UART_DeInit(&huart1);

      qbufferCreate(&qbuffer[ch], &rx_buf[0], 256);

      __HAL_RCC_DMA2_CLK_ENABLE();
      HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);


      if (HAL_UART_Init(&huart1) != HAL_OK)
      {
        ret = false;
      }
      else
      {
        ret = true;
        is_open[ch] = true; // 플래그 바꿔주기, 초기화 성공

        HAL_StatusTypeDef HAL_UART_Receive_DMA(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size);


        if(HAL_UART_Receive_DMA(&huart1, (uint8_t *)&rx_buf[0], 256) != HAL_OK)
          {
            ret = false;
          }

        qbuffer[ch].in = (qbuffer[ch].len - hdma_usart1_rx.Instance->NDTR);
        qbuffer[ch].out = qbuffer[ch].in;

      }
      break;
  }
  return ret;
}

uint32_t uartAvailable(uint8_t ch)
{
  uint32_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
      ret = cdcAvailable();
      break;

    case _DEF_UART2:
      qbuffer[ch].in = (qbuffer[ch].len - hdma_usart1_rx.Instance->NDTR);
      ret = qbufferAvailable(&qbuffer[ch]);
      break;
  }

  return ret;
}

uint8_t  uartRead(uint8_t ch)
{
  uint8_t ret = 0;

  switch(ch)
  {
    case _DEF_UART1:
       ret = cdcRead();
       break;

    case _DEF_UART2:
      qbufferRead(&qbuffer[_DEF_UART2], &ret, 1);
      break;
  }

  return ret;
}

uint32_t uartWrite(uint8_t ch, uint8_t *p_data, uint32_t length)
{
  uint32_t ret = 0;
  HAL_StatusTypeDef status;

  switch(ch)
  {
    case _DEF_UART1:
      ret = cdcWrite(p_data, length);
      break;

    case _DEF_UART2:
      status = HAL_UART_Transmit(&huart1, p_data, length, 100);
      if (status == HAL_OK)
      {
        ret = length; //보낸 것의 사이즈를 리턴해줘야함
      }
      break;
  }

  return ret;
}

uint32_t uartPrintf(uint8_t ch, char *fmt, ...)
{
  char buf[256];   //가변인자의 string처리를 위해 버퍼를 잡는다
  va_list args;
  int len;
  uint32_t ret;

  va_start(args, fmt);
  len = vsnprintf(buf, 256, fmt, args); //버퍼에 가변인자 string을 받아서 처리하는 것

  ret = uartWrite(ch, (uint8_t *)buf, len);

  va_end(args);


  return ret;
}

uint32_t uartGetBaud(uint8_t ch)
{
  uint32_t ret = 0;


  switch(ch)
  {
    case _DEF_UART1:
      ret = cdcGetBaud();
      break;

    case _DEF_UART2:
      ret = huart1.Init.BaudRate;
      break;
  }

  return ret;
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART1)
  {
  }
}



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
#if 0
  if (huart->Instance == USART1)
  {
    qbufferWrite(&qbuffer[_DEF_UART2], &rx_data[_DEF_UART2], 1);

    HAL_UART_Receive_IT(&huart1, (uint8_t *)&rx_data[_DEF_UART2], 1);
  } // 따로 에러처리는 하지 않을 것
#endif
}




void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 DMA Init */
    /* USART1_RX Init */
    hdma_usart1_rx.Instance = DMA2_Stream2;
    hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
    hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_usart1_rx) != HAL_OK)
    {
      Error_Handler();
    }

    __HAL_LINKDMA(uartHandle,hdmarx,hdma_usart1_rx);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* USART1 DMA DeInit */
    HAL_DMA_DeInit(uartHandle->hdmarx);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}
