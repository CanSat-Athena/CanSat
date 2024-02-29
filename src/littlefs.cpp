/* Copyright (C) 1883 Thomas Edison - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD 3 clause license, which unfortunately
 * won't be written for another century.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * A little flash file system for the Raspberry Pico
 *
 */

#include "littlefs.hpp"
#include <cstring>

 // Pico specific hardware abstraction functions
static int flash_fs_read(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int flash_fs_prog(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int flash_fs_erase(const struct lfs_config* config, lfs_block_t block);
static int flash_fs_sync(const struct lfs_config* config);
static int flash_fs_unlock(const struct lfs_config* config);
static int flash_fs_lock(const struct lfs_config* config);

struct lfs_config pico_cfg {
    // block device operations
    .read = &flash_fs_read,
        .prog = &flash_fs_prog,
        .erase = &flash_fs_erase,
        .sync = &flash_fs_sync,
        .lock = &flash_fs_lock,
        .unlock = &flash_fs_unlock,

        // block device configuration
        .read_size = 1,
        .prog_size = FLASH_PAGE_SIZE,
        .block_size = FLASH_SECTOR_SIZE,
        .block_count = FS_SIZE / FLASH_SECTOR_SIZE,
        .block_cycles = (int32_t)500,
        .cache_size = 256,
        .lookahead_size = 32
};

struct flashProgram_t {
    uint32_t addr;
    const uint8_t* buffer;
    size_t size;
} flashProgram_t;

struct flashErase_t {
    uint32_t flashOffset;
    size_t size;
} flashErase_t;

SemaphoreHandle_t filesystemMutex;

static int flash_fs_read(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) {
    uint32_t fs_start = XIP_BASE + HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * config->block_size) + off;

    memcpy(buffer, (unsigned char*)addr, size);
    return LFS_ERR_OK;
}

static void flashProgramFunc(void *param) {
    struct flashProgram_t *fp = (struct flashProgram_t*)param;
    flash_range_program(fp->addr, fp->buffer, fp->size);
}

static void flashEraseFunc(void *param) {
    struct flashErase_t *fe = (struct flashErase_t*)param;
    flash_range_erase(fe->flashOffset, fe->size);
}

static int flash_fs_prog(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size) {
    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * config->block_size) + off;

    // printf("[FS] WRITE: %p, %d\n", addr, size);
    struct flashProgram_t fp = {
        .addr = addr,
        .buffer = (const uint8_t*)buffer,
        .size = size
    };

    flash_safe_execute(flashProgramFunc, &fp, UINT32_MAX);

    return LFS_ERR_OK;
}

static int flash_fs_erase(const struct lfs_config* config, lfs_block_t block) {
    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t offset = fs_start + (block * config->block_size);

    // printf("[FS] ERASE: %p\n", offset);
    struct flashErase_t fe = {
        .flashOffset = offset,
        .size = config->block_size
    };

    flash_safe_execute(flashEraseFunc, &fe, UINT32_MAX);

    return LFS_ERR_OK;
}

static int flash_fs_sync(const struct lfs_config* config) {
    return LFS_ERR_OK;
}

static int flash_fs_lock(const struct lfs_config* config) {
    if (xSemaphoreTake(filesystemMutex, (TickType_t)20) == pdTRUE) {
        return LFS_ERR_OK;
    } else {
        return -1;
    }
}

static int flash_fs_unlock(const struct lfs_config* config) {
    if (xSemaphoreGive(filesystemMutex) == pdTRUE) {
        return LFS_ERR_OK;
    } else {
        return -1;
    }
}