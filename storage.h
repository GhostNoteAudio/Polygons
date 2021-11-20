#pragma once

#include "Polygons.h"
#include <SdFat.h>
#include <stdint.h>

namespace Polygons
{
    namespace Storage
    {
        extern SdFat sd;
        extern char filePathBuffer[256];

        bool InitStorage();
        bool DirExists(const char* dirPath);
        bool FileExists(const char* filePath);
        void CreateFolder(const char* dirPath);
        int GetFileCount(const char* dirPath);
        const char* GetFilePath(const char* dirPath, int index);
        bool WriteFile(const char* filePath, uint8_t* data, int dataLen);
        int GetFileSize(const char* filePath);
        void ReadFile(const char* filePath, uint8_t* data, int maxDataLen);
    }
}
