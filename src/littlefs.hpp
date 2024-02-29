#pragma once
#include <limits.h>
#include "hardware/flash.h"
#include "pico/flash.h"
#include "hardware/regs/addressmap.h"
#include <FreeRTOS.h>
#include <semphr.h>
#include "hardware/sync.h"
#include "pico/time.h"
#include "littlefs/lfs.h"

#include "config.h"
#include "streamHandler.h"

#define BLOCK_SIZE_BYTES (FLASH_SECTOR_SIZE)
#define HW_FLASH_STORAGE_BYTES  FS_SIZE
#define HW_FLASH_STORAGE_BASE   (PICO_FLASH_SIZE_BYTES - HW_FLASH_STORAGE_BYTES) // 655360

extern struct lfs_config pico_cfg;
extern SemaphoreHandle_t filesystemMutex;