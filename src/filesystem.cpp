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

        static const char* prefixes[] = { "", "K", "M", "G" };
        for (int i = sizeof(prefixes) / sizeof(prefixes[0]) - 1; i >= 0; i--) {
            if (info.size >= (1 << 10 * i) - 1) {
                printf("%*u%sB ", 4 - (i != 0), info.size >> 10 * i, prefixes[i]);
                break;
            }
        }

        // Print file
        printf("%s\n", info.name);
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

#ifdef NUKE_FS_ON_NEXT_BOOT
    printf("Filesystem: Reformatting...\n");
    lfs_format(&lfs, &pico_cfg);
#endif

    // mount the filesystem
    int err = lfs_mount(&lfs, &pico_cfg);

    if (err) {
        printf("Filesystem: Failed to mount\n");
        printf("Filesystem: Reformatting...\n");
        lfs_format(&lfs, &pico_cfg);
        err = lfs_mount(&lfs, &pico_cfg);
    }

#ifdef NUKE_FS_ON_NEXT_BOOT
    ls("/");
    printf("Filesystem: Nuked");
    while (true) tight_loop_contents();
#endif

    printf("Filesystem: Initialised\n");

    lfs_file_open(&lfs, &bootCountFile, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
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
    printf("Filesystem: Created file %s\n", dataFileName);
    lfs_file_open(&lfs, &dataFile, dataFileName, LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC);
    lfs_file_sync(&lfs, &dataFile);

    ls("/");

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
 
int Filesystem::addData(dataLine_t data) {
    // lfs_file_write(&lfs, &dataFile, &data, sizeof(dataLine_t));
    char a[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    // int err = lfs_file_write(&lfs, &dataFile, &a, strlen(a+1));
    // printf("err: %d\n", err);
    // err = lfs_file_sync(&lfs, &dataFile);
    return 0;
}