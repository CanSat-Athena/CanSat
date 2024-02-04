#pragma once
#include "pico/stdio.h"

#include "littlefs.hpp"
#include "config.h"

class Filesystem {
protected:
    bool initialised = false;
    uint32_t bootCount = 0;
    lfs_t lfs;

    lfs_file_t bootCountFile;
    lfs_file_t bootLogFile;         // Yet to be implemented
    lfs_file_t dataFile;

public:
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
};