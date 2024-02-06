#pragma once
#include <pico/stdio.h>

#include "littlefs.hpp"
#include "config.h"
#include "commonTypes.h"

class Filesystem {
protected:
    bool initialised = false;

    lfs_file_t bootCountFile;
    uint8_t bootCountBuf[256];
    lfs_file_t bootLogFile;         // Yet to be implemented

    char dataFileName[50];

public:
    lfs_t lfs;
    lfs_file_t dataFile;
    uint32_t bootCount = 0;

    Filesystem(const bool initialise = true) {
        init();
    }

    ~Filesystem() {
        this->uninit();
    }

    /// @brief Initialises (mounts) the filesystem
    /// @return True if successful
    bool init();

    /// @brief Uninitialises (unmounts) the filesystem
    void uninit();

    /// @brief Erases the filesystem
    void nuke();

    /// @brief Lists the directory given
    /// @param path String path to directory
    /// @return Any errors received
    int ls(const char* path);

    /// @brief Adds data to the data file
    /// @param data The line to write
    /// @return The number of bytes written, or a negative error code on failure.
    int addData(dataLine_t data);

    static void filesystemHandler();
};