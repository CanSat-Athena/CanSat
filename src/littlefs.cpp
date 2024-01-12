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

 // Pico specific hardware abstraction functions
static int flash_fs_read(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
static int flash_fs_prog(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
static int flash_fs_erase(const struct lfs_config* config, lfs_block_t block);
static int flash_fs_sync(const struct lfs_config* config);


struct lfs_config pico_cfg {
    // block device operations
    .read = &flash_fs_read,
    .prog = &flash_fs_prog,
    .erase = &flash_fs_erase,
    .sync = &flash_fs_sync,

    // block device configuration
    .read_size = 1,
    .prog_size = FLASH_PAGE_SIZE,
    .block_size = FLASH_SECTOR_SIZE,
    .block_count = FS_SIZE / FLASH_SECTOR_SIZE,
    .block_cycles = (int32_t)500,
    .cache_size = FLASH_SECTOR_SIZE / 4,
    .lookahead_size = 32
};

static int flash_fs_read(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) {
    uint32_t fs_start = XIP_BASE + HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * config->block_size) + off;
        
    memcpy(buffer, (unsigned char *)addr, size);
    return LFS_ERR_OK;
}

static int flash_fs_prog(const struct lfs_config* config, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size) {
    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t addr = fs_start + (block * config->block_size) + off;
    
    // printf("[FS] WRITE: %p, %d\n", addr, size);
        
    uint32_t ints = save_and_disable_interrupts();
    flash_range_program(addr, (const uint8_t *)buffer, size);
    restore_interrupts(ints);

    return LFS_ERR_OK;
}

static int flash_fs_erase(const struct lfs_config* config, lfs_block_t block) {
    uint32_t fs_start = HW_FLASH_STORAGE_BASE;
    uint32_t offset = fs_start + (block * config->block_size);
            
    uint32_t ints = save_and_disable_interrupts();   
    flash_range_erase(offset, config->block_size);  
    restore_interrupts(ints);

    return LFS_ERR_OK;
}

static int flash_fs_sync(const struct lfs_config* config) {
    return LFS_ERR_OK;
}