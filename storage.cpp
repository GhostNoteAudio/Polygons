#include "storage.h"
#include "PolygonsBase.h"

namespace Polygons
{
    namespace Storage
    {
        SdFat sd;
        char filePathBuffer[256];

        bool InitStorage()
        {
            if (!sd.begin(P_SPI_SD_CS, SPI_HALF_SPEED))
            {
                sd.initErrorHalt(&Serial);
                return false;
            }
            Serial.println("SD Card initialization done.");
            return true;
        }

        bool DirExists(const char* dirPath)
        {
            if (!sd.exists(dirPath))
                return false;
            
            SdFile dir;
            if (!dir.open(dirPath))
            {
                sd.errorHalt("Failed to open directory");
                return false;
            }

            return dir.isDir();
        }

        bool FileExists(const char* filePath)
        {
            if (!sd.exists(filePath))
                return false;
            
            SdFile file;
            if (!file.open(filePath))
            {
                sd.errorHalt("Failed to open file");
                return false;
            }

            return file.isFile();
        }

        void CreateFolder(const char* dirPath)
        {
            sd.mkdir(dirPath);
        }

        int GetFileCount(const char* dirPath)
        {
            SdFile dir, file;
            if (!dir.open(dirPath))
            {
                sd.errorHalt("Failed to open directory");
                return -1;
            }

            if (!dir.isDir())
                return -1;

            int count = 0;
            while (file.openNext(&dir, O_RDONLY)) 
            {
                count++;
                file.close();
            }
            if (dir.getError()) 
            {
                Serial.println("Failed to list directory");
                return -1;
            }
            return count;
        }

        const char* GetFilePath(const char* dirPath, int index)
        {
            strcpy(filePathBuffer, "");

            SdFile dir, file;
            if (!dir.open(dirPath))
            {
                sd.errorHalt("Failed to open directory");
                return filePathBuffer;
            }

            if (!dir.isDir())
            {
                return filePathBuffer;
            }

            int count = 0;
            while (file.openNext(&dir, O_RDONLY)) 
            {
                if (count == index)
                {
                    strcpy(filePathBuffer, dirPath);
                    filePathBuffer[strlen(dirPath)] = '/';
                    file.getName(&filePathBuffer[strlen(dirPath) + 1], sizeof(filePathBuffer));
                    file.close();
                    return filePathBuffer;
                }
                file.close();
                count++;
            }

            if (dir.getError()) 
            {
                Serial.println("Failed to list directory");
                return filePathBuffer;
            }

            return filePathBuffer;
        }

        bool WriteFile(const char* filePath, uint8_t* data, int dataLen)
        {
            SdFile myFile;

            if (sd.exists(filePath)) 
            {
                Serial.print("Removing existing file before writing: ");
                Serial.println(filePath);
                sd.remove(filePath);
            }
            
            if (!myFile.open(filePath, FILE_WRITE)) 
            {
                Serial.println("open failed");
                return false;
            }

            auto result = myFile.write(data, dataLen);
            myFile.sync();
            return (int)result == dataLen;
        }

        int GetFileSize(const char* filePath)
        {
            SdFile myFile;
            if (!myFile.open(filePath))
            {
                sd.errorHalt("Failed to open file");
                return -1;
            }
            int output = myFile.available();
            return output;
        }

        void ReadFile(const char* filePath, uint8_t* data, int maxDataLen)
        {
            SdFile myFile;
            if (!myFile.open(filePath))
            {
                sd.errorHalt("Failed to open file");
                return;
            }

            int read_count = 0;
            while(true)
            {
                int b = myFile.read();
                if (b == -1)
                    break;
                data[read_count] = b;
                read_count++;
                if (read_count >= maxDataLen)
                    break;
            }
        }
    }
}
