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

    bool init();
    void uninit();
    void nuke(bool loop = true);

    int addData(dataLine_t data);

    int ls(const char* path);    
    void printUsage();
    void printSize(lfs_ssize_t size);
    void readFile(uint32_t bootCount);

    static void filesystemInputTask(void* pvParameters);
    static void filesystemNukeTask(void* pvParameters);
    static void filesystemDeleteTask(void* pvParameters);
};