#include "Polygons.h"

using namespace Polygons::Storage;

void setup() 
{
    Serial.begin(115200);
    while (!Serial)
    {
    }
    
    Serial.println ("Serial communication available");
    delay (1000);
    uint8_t data[256];
    
    InitStorage();
    int count = GetFileCount("folder1");
    for (size_t i = 0; i < count; i++)
    {
        const char* fp = GetFilePath("folder1", i);
        Serial.println(fp);
    }
    
    const char* filepath = GetFilePath("folder1", 2);
    WriteFile(filepath, (uint8_t*)"Hello world", 11);
    ReadFile(filepath, data, 256);
    Serial.print("file count: ");
    Serial.println(count);
    Serial.print("file path: ");
    Serial.println(filepath);
    Serial.print("file content: ");
    Serial.println((char*)data);
}

void loop()
{  

}