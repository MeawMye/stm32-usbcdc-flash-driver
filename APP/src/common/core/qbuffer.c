/*
 * qbuffer..c
 *
 *  Created on: Aug 17, 2025
 *      Author: y6484
 */


#include "qbuffer.h"





void     qbufferInit(void)
{

}

bool qbufferCreate(qbuffer_t *p_node, uint8_t *p_buf, uint32_t length)
{
  bool ret = true;


  p_node->in     = 0;
  p_node->out    = 0;
  p_node->len    = length;
  p_node->p_buf  = p_buf;

  return ret;
}

bool qbufferWrite(qbuffer_t *p_node, uint8_t *p_data, uint32_t length)
{
  bool ret = true;
  uint32_t next_in;


  for (int i=0; i<length; i++)
  {
     next_in = (p_node->in + 1) % p_node->len; //다음것을 계산할 수 있도록

     if (next_in != p_node->out)
     {
       if (p_node->p_buf != NULL)
       {
         p_node->p_buf[p_node->in] = p_data[i];
       }
       p_node->in = next_in;
     }
     else
     {
       ret = false;
       break;
     }
  }

  return ret;

}

bool qbufferRead(qbuffer_t *p_node, uint8_t *p_data, uint32_t length)
{
  bool ret = true;


  for (int i=0; i<length; i++)
  {
    if (p_node->p_buf != NULL)
    {
      p_data[i] = p_node->p_buf[p_node->out];
    }

    if (p_node->out != p_node->in) // in == out 이라면 이미 비어있는 곳을 읽어가려하는 것
    {
      p_node->out = (p_node->out + 1) % p_node->len;
    }
    else
    {
      ret = false;
      break;
    }
  }
  return ret;
}

uint32_t qbufferAvailable(qbuffer_t *p_node)
{
  uint32_t ret;


  ret = (p_node->in - p_node->out) % p_node->len;

  return ret;
}

void qbufferFlush(qbuffer_t *p_node)
{
  p_node->in = 0;
  p_node->out = 0;
}
