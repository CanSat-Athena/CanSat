#include "filesystem.hpp"

int Filesystem::ls(const char* path) {
    lfs_dir_t dir;
    int err = lfs_dir_open(&lfs, &dir, path);
    if (err) {
        return err;
    }

    struct lfs_info info;

    // Loop over each file
    while (true) {
        int res = lfs_dir_read(&lfs, &dir, &info);
        if (res < 0) {
            return res;
        }

        if (res == 0) {
            break;
        }

        // Get if file, directory or other
        switch (info.type) {
        case LFS_TYPE_REG: printf("reg "); break;
        case LFS_TYPE_DIR: printf("dir "); break;
        default:           printf("?   "); break;
        }

        printSize(info.size);

        // Print file
        printf(" %s\n", info.name);
    }

    // Close directory
    err = lfs_dir_close(&lfs, &dir);
    if (err) {
        return err;
    }

    return 0;
}

bool Filesystem::init() {
    printf("Filesystem: Setting up\n");

    printf("Filesystem: Initialising mutex\n");
    filesystemMutex = xSemaphoreCreateMutex();

#ifdef NUKE_FS_ON_NEXT_BOOT
    printf("Filesystem: Reformatting...\n");
    lfs_format(&lfs, &pico_cfg);
#endif

    // mount the filesystem
    int err = lfs_mount(&lfs, &pico_cfg);

    if (err) {
        printf("Filesystem: Failed to mount\n");
        printf("Filesystem: Formatting in 60 seconds \n");

        // Wait 60 seconds
        sleep_ms(60 * 1000);

        printf("Filesystem: Reformatting...\n");
        lfs_format(&lfs, &pico_cfg);
        err = lfs_mount(&lfs, &pico_cfg);
    }

#ifdef NUKE_FS_ON_NEXT_BOOT
    ls("/");
    printf("Filesystem: Nuked");
    while (true) tight_loop_contents();
#endif

    printf("Filesystem: Mounted\n");

    static struct lfs_file_config bootCountConfig;
    bootCountConfig.buffer = bootCountBuf;  // use the static buffer
    bootCountConfig.attr_count = 0;

    lfs_file_opencfg(&lfs, &bootCountFile, "boot_count", LFS_O_RDWR | LFS_O_CREAT, &bootCountConfig);
    lfs_file_read(&lfs, &bootCountFile, &(this->bootCount), sizeof(bootCount));

    // update boot count
    bootCount += 1;
    lfs_file_rewind(&lfs, &bootCountFile);
    lfs_file_write(&lfs, &bootCountFile, &bootCount, sizeof(bootCount));

    // The storage is not updated until the file is closed successfully (or synced)
    lfs_file_close(&lfs, &bootCountFile);
    printf("Filesystem: boot_count updated, new boot count is %u\n", bootCount);

    // Create data file
    sprintf(dataFileName, "data_%u", this->bootCount);

    lfs_file_open(&lfs, &dataFile, dataFileName, LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC);
    lfs_file_sync(&lfs, &dataFile);

    printf("Filesystem: Created file %s\n", dataFileName);
    ls("/");
    printUsage();

    printf("Filesystem: Initialised\n");

    this->initialised = true;
    return true;
}

void Filesystem::nuke() {
    printf("Filesystem: nuking the filesystem...\n");
    lfs_format(&lfs, &pico_cfg);
    printf("Filesystem: Filesystem has been nuked. \nFilesystem: Please re-upload program with NUKE_FS_ON_NEXT_BOOT removed from config.h to continue normal operation\n");
    while (true) {
        tight_loop_contents();
    }
}

void Filesystem::uninit() {
    // Unmount filesystem
    lfs_unmount(&lfs);

    this->initialised = false;
}

void Filesystem::printUsage() {
    lfs_ssize_t size = lfs_fs_size(&lfs);
    uint32_t size_bytes = size * BLOCK_SIZE_BYTES;
    float usage = (float)size_bytes / (float)FS_SIZE;

    printf("Filesystem: Used/total: ");
    printSize(size_bytes);
    printf("/");
    printSize(FS_SIZE);
    printf(" (%.2f%%)\n", usage * 100);
}

void Filesystem::printSize(lfs_ssize_t size) {
    static const char* prefixes[] = { "", "K", "M", "G" };
    for (int i = sizeof(prefixes) / sizeof(prefixes[0]) - 1; i >= 0; i--) {
        if (size >= (1 << 10 * i) - 1) {
            printf("%*u%sB", 4 - (i != 0), size >> 10 * i, prefixes[i]);
            break;
        }
    }
}

int Filesystem::addData(dataLine_t data) {
    lfs_file_write(&lfs, &dataFile, &data, sizeof(dataLine_t));
    int err = lfs_file_sync(&lfs, &dataFile);
    return err;
}