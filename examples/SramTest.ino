#include "Polygons.h"

byte data[128];
byte data2[128];

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);
    Polygons::init();

    Polygons::codec.lineOutGain(0, 0, false);

    for(uint i=0; i<sizeof(data); i++)
    {
        data[i] = i;
    }

    uint32_t address = 0;
    Serial.begin(115200); 
    SPI.begin();
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    int a = micros();

    Polygons::sram.WriteByteArray(address, data, sizeof(data));

    int b = micros();

    Polygons::sram.ReadByteArray(address, data2, sizeof(data2)); 

    int c = micros();

    SPI.endTransaction();
    Serial.print("Write time: ");
    Serial.print(b-a);
    Serial.println(" microseconds");

    Serial.print("Read time: ");
    Serial.print(c-b);
    Serial.println(" microseconds");

    for(uint i=0; i<sizeof(data2); i++)
    {
        Serial.println(data2[i]);
    }
}

void loop()
{

}
