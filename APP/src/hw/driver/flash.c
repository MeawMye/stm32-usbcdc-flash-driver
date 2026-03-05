/*
 * flash.c
 *
 *  Created on: Aug 21, 2025
 *      Author: user
 */


#include "flash.h"


#define FLASH_SECTOR_MAX      24

typedef struct
{
  uint32_t sector;
  uint32_t base;
  uint32_t size;
} FlashSectorInfo;



static const FlashSectorInfo FlashSector[FLASH_SECTOR_MAX] =
    {
        { FLASH_SECTOR_0,  0x08000000,  16*1024 },
        { FLASH_SECTOR_1,  0x08004000,  16*1024 },
        { FLASH_SECTOR_2,  0x08008000,  16*1024 },
        { FLASH_SECTOR_3,  0x0800C000,  16*1024 },
        { FLASH_SECTOR_4,  0x08010000,  64*1024 },
        { FLASH_SECTOR_5,  0x08020000, 128*1024 },
        { FLASH_SECTOR_6,  0x08040000, 128*1024 },
        { FLASH_SECTOR_7,  0x08060000, 128*1024 },
        { FLASH_SECTOR_8,  0x08080000, 128*1024 },
        { FLASH_SECTOR_9,  0x080A0000, 128*1024 },
        { FLASH_SECTOR_10, 0x080C0000, 128*1024 },
        { FLASH_SECTOR_11, 0x080E0000, 128*1024 },
        { FLASH_SECTOR_12,  0x08100000,  16*1024 }, // 여기서부터 bank2
        { FLASH_SECTOR_13,  0x08104000,  16*1024 },
        { FLASH_SECTOR_14,  0x08108000,  16*1024 },
        { FLASH_SECTOR_15,  0x0810C000,  16*1024 },
        { FLASH_SECTOR_16,  0x08110000,  64*1024 },
        { FLASH_SECTOR_17,  0x08120000, 128*1024 },
        { FLASH_SECTOR_18,  0x08140000, 128*1024 },
        { FLASH_SECTOR_19,  0x08160000, 128*1024 },
        { FLASH_SECTOR_20,  0x08180000, 128*1024 },
        { FLASH_SECTOR_21,  0x081A0000, 128*1024 },
        { FLASH_SECTOR_22, 0x081C0000, 128*1024 },
        { FLASH_SECTOR_23, 0x081E0000, 128*1024 },
    };


static uint32_t GetBank(uint32_t base);
static bool flashInSector(uint32_t sector, uint32_t base, uint32_t len);


bool flashErase(uint32_t base, uint32_t size)
{
  bool ret = false;

  HAL_StatusTypeDef status;
  FLASH_EraseInitTypeDef init;
  uint32_t sector_error;

  int start_sector_num = -1;
  uint32_t sector_count = 0;

  //uint32_t bank = GetBank(base);

  for (int i=0; i<FLASH_SECTOR_MAX; i++)
  {
    if (flashInSector(i, base, size) == true)
    {
      if (start_sector_num < 0)
      {
        start_sector_num = i;
      }
      sector_count++;
    }
  }

   if (sector_count > 0)
    {

      HAL_FLASH_Unlock();

      init.TypeErase    = FLASH_TYPEERASE_SECTORS;
      init.Banks        = GetBank(base);
      init.Sector       = FlashSector[start_sector_num].sector;
      init.NbSectors    = sector_count;
      init.VoltageRange = FLASH_VOLTAGE_RANGE_3;


      status = HAL_FLASHEx_Erase(&init, &sector_error);

      if ( status == HAL_OK )
      {
        ret = true;
      }

      HAL_FLASH_Lock();
    }
   return ret;
  }



bool flashWrite(uint32_t base, uint8_t *p_data, uint32_t size)
{
  bool ret = true;
  HAL_StatusTypeDef status;


  if ( base%2 != 0 )
  {
    return false; // 얼라인된 경우만 write 할수 있게
  }


  HAL_FLASH_Unlock();

  for (int i=0; i<size; i+=2)
  {
    uint16_t data;

    data = p_data[i+0] << 0;
    data |= p_data[i+1] << 8;


    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, base + i, (uint64_t)data);
    if ( status != HAL_OK ) // 현재 루프를 도는 중이므로 실패하면 빠져나오게끔
    {
      ret = false;
      break;
   }
  }


  HAL_FLASH_Lock();


  return ret;
}


bool flashRead(uint32_t base, uint8_t *p_data, uint32_t size)
{
  bool ret = true;

  uint8_t *p_byte = (uint8_t *)base;

  for (int i=0; i<size; i++)
  {
    p_data[i] = p_byte[i];
  }

  return ret;
}

bool flashInSector(uint32_t sector_index, uint32_t base, uint32_t size)  // 지우고자 하는 플래시의 전체 시작주소와 길이
{
  bool ret = false;

  uint32_t sector_start;
  uint32_t sector_end;
  uint32_t flash_start;
  uint32_t flash_end;

  sector_start = FlashSector[sector_index].base;
  sector_end = FlashSector[sector_index].base + FlashSector[sector_index].size - 1;
  flash_start = base;
  flash_end = base + size - 1;

  if (sector_start >= flash_start && sector_start <= flash_end)
  {
  ret = true;
  }

  if (sector_end >= flash_start && sector_end <= flash_end)
  {
  ret = true;
  }

  if (flash_start >= sector_start && flash_start <= sector_end)
  {
  ret = true;
  }

  if (flash_end >= sector_start && flash_end <= sector_end)
  {
  ret = true;
  }

  return ret;
}


uint32_t GetBank(uint32_t base)
{
  if (base < 0x08100000)
      return FLASH_BANK_1;
  else
      return FLASH_BANK_2;
}

/*
static inline const flashFindSector(uint32_t addr)
{
  bool ret = false;

  for (int i=0; i<FLASH_SECTOR_MAX; i++)
    {
      uint32_t base = FlashSector[i].base;
      uint32_t end = base + FlashSector[i].size;
      if ( addr >= base && addr < end )
        ret = FlashSector[i].sector;
        return ret;
    }

  return ret;
} */
