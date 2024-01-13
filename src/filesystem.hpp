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
        this->destroy();
    }

    /// @brief Initialises (mounts) the filesystem
    /// @return True if successful
    bool init();

    /// @brief Uninitialises (unmounts) the filesystem
    void destroy();

    /// @brief Erases the filesystem
    void nuke();
};