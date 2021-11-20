#include "Polygons.h"
#include "blocks/DelayBlockExternal.h"

DMAMEM DelayBlockExternal<32768, AUDIO_BLOCK_SAMPLES> d1(0);
int32_t buffer1[5] = {11,22,33,44,55};
int32_t buffer2[5] = {0};

void setup()
{
    Serial.begin(115200);
    Polygons::init();    
    d1.init();

    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    d1.write(buffer1, 5);
    d1.updatePtr(5);
    //SPI.endTransaction();
    //delay(1);

    //SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    d1.read(buffer2, 5, 5);
    SPI.endTransaction();

    Serial.println("Values1: ");
    Serial.println(buffer1[0]);
    Serial.println(buffer1[1]);
    Serial.println(buffer1[2]);
    Serial.println(buffer1[3]);
    Serial.println(buffer1[4]);

    Serial.println("Values2: ");
    Serial.println(buffer2[0]);
    Serial.println(buffer2[1]);
    Serial.println(buffer2[2]);
    Serial.println(buffer2[3]);
    Serial.println(buffer2[4]);
}

void loop()
{

}
