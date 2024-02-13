#include "filesystem.hpp"
#include "globals.h"

/// @brief Lists the directory given
/// @param path String path to directory
/// @return Any errors received
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

/// @brief Initialises (mounts) the filesystem
/// @return True if successful
bool Filesystem::init() {
    printf("Filesystem: Setting up\n");

    printf("Filesystem: Initialising mutex\n");
    filesystemMutex = xSemaphoreCreateMutex();

#ifdef NUKE_FS_ON_NEXT_BOOT
    // Launch the ICBMs
    nuke();
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
    printf("Filesystem: ");
    printUsage();

    // Start filesystem input task
    printf("Filesystem: Starting input task\n");
    xTaskCreate(filesystemInputTask, "Filesystem input", 1024, NULL, 1, NULL);

    printf("Filesystem: Initialised\n");
    this->initialised = true;
    return true;
}

/// @brief Launches an intercontinental ballistic missile headed towards the filesystem
void Filesystem::nuke(bool loop) {
    printf("Filesystem: Nuking the filesystem...\n");
    lfs_format(&lfs, &pico_cfg);
    if (loop) printf("Filesystem: Filesystem has been nuked. \nFilesystem: Please re-upload program with NUKE_FS_ON_NEXT_BOOT removed from config.h to continue normal operation\n");
    while (loop) {
        tight_loop_contents();
    }
}

/// @brief Uninitialises (unmounts) the filesystem
void Filesystem::uninit() {
    // Unmount filesystem
    lfs_unmount(&lfs);

    this->initialised = false;
}

/// @brief Prints filesystem usage
void Filesystem::printUsage() {
    lfs_ssize_t size = lfs_fs_size(&lfs);
    uint32_t size_bytes = size * BLOCK_SIZE_BYTES;
    float usage = (float)size_bytes / (float)FS_SIZE;

    printf("Used/total: ");
    printSize(size_bytes);
    printf("/");
    printSize(FS_SIZE);
    printf(" (%.2f%%)\n", usage * 100);
}

/// @brief Prints lfs_size_t parameter as an automatically converted size (eg B, KB, MB, GB)
/// @param size The size in bytes to print
void Filesystem::printSize(lfs_ssize_t size) {
    static const char* prefixes[] = { "", "K", "M", "G" };
    for (int i = sizeof(prefixes) / sizeof(prefixes[0]) - 1; i >= 0; i--) {
        if (size >= (1 << 10 * i) - 1) {
            printf("%*u%sB", 4 - (i != 0), size >> 10 * i, prefixes[i]);
            break;
        }
    }
}

/// @brief Adds data to the data file
/// @param data The line to write
/// @return The number of bytes written, or a negative error code on failure.
int Filesystem::addData(dataLine_t data) {
    lfs_file_write(&lfs, &dataFile, &data, sizeof(dataLine_t));
    int err = lfs_file_sync(&lfs, &dataFile);
    return err;
}

/// @brief Prints a file to the console
/// @param bootCount Boot count of file to print
void Filesystem::readFile(uint32_t bootCount) {
    char dataFileName[50];
    sprintf(dataFileName, "data_%u", bootCount);

    lfs_file_t dataFile;
    int err = lfs_file_open(&lfs, &dataFile, dataFileName, LFS_O_RDONLY);

    // Handle errors
    if (err == LFS_ERR_NOENT) {
        printf("Error: File %s does not exist\n", dataFileName);
        return;
    } else if (err < 0) {
        printf("Error: Failed to open file %s\n", dataFileName);
        return;
    }

    printf("Printing file %s:\n", dataFileName);

    lfs_ssize_t readStatus = 0;
    dataLine_t line;

    // Print each line
    while (true) {
        readStatus = lfs_file_read(&lfs, &dataFile, (void*)(&line), sizeof(dataLine_t));
        if (readStatus < sizeof(dataLine_t)) break;

        printf("%u: ", line.timestamp);

        // Get DHT20 data
        printf("[");
        for (int i = 0; i < DHT20_READ_FREQ; i++) {
            printf("[%f, %f], ", line.dht20[i].temperature, line.dht20[i].humidity);
        }
        printf("], ");

        // Get BME680 data
        printf("[");
        for (int i = 0; i < BME680_READ_FREQ; i++) {
            printf("[%f, %f, %f, %f], ",
                line.bme680[i].temperature,
                line.bme680[i].humidity,
                line.bme680[i].pressure,
                line.bme680[i].gasResistance
            );
        }
        printf("], ");

        // Get light data
        printf("[");
        for (int i = 0; i < LIGHT_READ_FREQ; i++) {
            printf("%u, ", line.lightData[i].lightIntensity);
        }
        printf("], ");

        // Get GPS data
        printf("[");
        printf("%f, %f, %f, %u", line.gpsData[0].latitude, line.gpsData[0].longitude, line.gpsData[0].altitude, line.gpsData[0].fix);
        printf("], ");

        printf("\n");
    }

    // Don't forget to close
    lfs_file_close(&lfs, &dataFile);
}

/// @brief Task to handle filesystem input from USB
/// @param pvParameters unused
void Filesystem::filesystemInputTask(void* pvParameters) {
    char c;
    char fileBootCount[45];

    char helpText[] = "Commands:\n"
        "'p' to print a data file\n"
        "'l' to list directory\n"
        "'u' to show filesystem usage\n"
        "'d' to delete a file\n"
        "'n' to nuke the filesystem\n"
        "'h' to display this message\n";
    printf(helpText);

    while (true) {
        vTaskDelay(40);

        printf("$ ");
        char c = getchar();

        switch (c) {
        case 'p':
            printf("d\nEnter boot count: ");
            dataHandler->filesystem->readFile(getIntInput());
            break;
        case 'n': {
            // Get confirmation
            printf("Press 'Y' (uppercase) to confirm nuke: ");
            char c2 = getchar();
            if (c2 == 'Y') {
                printf("\n");

                // Launch the ICBMs
                xTaskCreate(filesystemNukeTask, "Filesystem nuke", 512, NULL, 20, NULL);
            } else {
                printf("\nAborting nuclear operation\n");
            }
            break;
        }
        case 'd': {
            printf("d\nEnter boot count of file to delete: ");
            uint32_t bootCount = getIntInput();

            char dataFileName[50];
            sprintf(dataFileName, "data_%u", bootCount);
            printf("Will delete file: %s\n", dataFileName);

            // Get confirmation
            printf("Press 'Y' (uppercase) to confirm delete: ");
            char c2 = getchar();
            if (c2 == 'Y') {
                printf("\n");

                // Delete file
                xTaskCreate(filesystemDeleteTask, "File delete", 512, &dataFileName, 21, NULL);
            } else {
                printf("\nAborting file delete\n");
            }
            break;
        }
        case 'l':
            printf("Listing directory \"/\"\n");
            dataHandler->filesystem->ls("/");
            break;
        case 'u':
            printf("Filesystem usage: ");
            dataHandler->filesystem->printUsage();
            break;
        case 'h':
            printf(helpText);
            break;
        default:
            printf("Invalid command: '%c'. Press 'h' for help\n", c);
            break;
        }
    }
}

/// @brief When run, safely nukes the filesystem.
///        Note: must be run with higher priority than other filesystem tasks
/// @param pvParameters unused
void Filesystem::filesystemNukeTask(void* pvParameters) {
    lfs_file_close(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile));
    lfs_unmount(&(dataHandler->filesystem->lfs));
    dataHandler->filesystem->nuke(false);
    lfs_mount(&(dataHandler->filesystem->lfs), &pico_cfg);

    lfs_file_open(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->bootCountFile), "boot_count", LFS_O_RDWR | LFS_O_CREAT);
    lfs_file_read(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->bootCountFile), &(dataHandler->filesystem->bootCount), sizeof(dataHandler->filesystem->bootCount));

    // update boot count
    (dataHandler->filesystem->bootCount) = 1;
    lfs_file_rewind(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->bootCountFile));
    lfs_file_write(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->bootCountFile), &(dataHandler->filesystem->bootCount), sizeof(dataHandler->filesystem->bootCount));

    // The storage is not updated until the file is closed successfully (or synced)
    lfs_file_close(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->bootCountFile));
    printf("Filesystem: boot_count updated, new boot count is %u\n", (dataHandler->filesystem->bootCount));

    // Create data file
    sprintf(dataHandler->filesystem->dataFileName, "data_%u", dataHandler->filesystem->bootCount);
    lfs_file_open(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile), dataHandler->filesystem->dataFileName, LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC);
    lfs_file_sync(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile));
    printf("Filesystem: Created file %s\n", dataHandler->filesystem->dataFileName);

    vTaskDelete(NULL);
}

/// @brief Deletes a file from the filesystem
///        Note: must be run with higher priority than other filesystem tasks
/// @param pvParameters The name of the file to delete
void Filesystem::filesystemDeleteTask(void* pvParameters) {
    char* dataFileName = (char*)pvParameters;

    bool isCurrentFile = strcmp(dataFileName, dataHandler->filesystem->dataFileName) == 0;

    if (isCurrentFile) {
        lfs_file_close(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile));
    }

    // Delete file
    int err = lfs_remove(&(dataHandler->filesystem->lfs), dataFileName);

    // Handle errors
    if (err == LFS_ERR_NOENT) {
        printf("Error: File %s does not exist\n", dataFileName);
    } else if (err < 0) {
        printf("Error: Failed to open file %s\n", dataFileName);
    } else {
        printf("Deleted file %s\n", dataFileName);
    }

    if (isCurrentFile) {
        lfs_file_open(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile), dataHandler->filesystem->dataFileName, LFS_O_RDWR | LFS_O_CREAT | LFS_O_TRUNC);
        lfs_file_sync(&(dataHandler->filesystem->lfs), &(dataHandler->filesystem->dataFile));
        printf("Created file %s\n", dataHandler->filesystem->dataFileName);
    }

    vTaskDelete(NULL);
}