#include "filesystem.hpp"

// void fs() {
//     lfs_t lfs;
//     lfs_file_t file;

// #ifdef NUKE_FS_ON_NEXT_BOOT
// #endif

//     printf("Setting up filesystem\n");

//     // mount the filesystem
//     int err = lfs_mount(&lfs, &pico_cfg);

//     // reformat if we can't mount the filesystem
//     // this should only happen on the first boot
//     if (err) {
//         printf("Reformatting...");
//         lfs_format(&lfs, &pico_cfg);
//         lfs_mount(&lfs, &pico_cfg);
//     }

//     // read current count
//     uint32_t boot_count = 0;
//     lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
//     lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

//     // update boot count
//     boot_count += 1;
//     lfs_file_rewind(&lfs, &file);
//     lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

//     // remember the storage is not updated until the file is closed successfully
//     lfs_file_close(&lfs, &file);

//     // release any resources used
//     lfs_unmount(&lfs);

//     // print the boot count
//     printf("boot_count: %d\n", boot_count);
// }

bool Filesystem::init() {
    printf("Filesystem: Setting up\n");

    // mount the filesystem
    int err = lfs_mount(&lfs, &pico_cfg);

    // reformat if we can't mount the filesystem
    // this should only happen on the first boot
    if (err) {
        printf("Filesystem: Reformatting...\n");
        lfs_format(&lfs, &pico_cfg);
        lfs_mount(&lfs, &pico_cfg);
    }

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