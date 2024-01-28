#pragma once
#include "pico/stdio.h"

#include "littlefs.hpp"

class Filesystem {
protected:
    bool initialised = false;
    lfs_t lfs;

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

};