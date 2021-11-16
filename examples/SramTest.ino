#include "SRAMsimple.h"
#include "Arduino.h"
#include "Tritium.h"


#define CSPIN 10       // Default Chip Select Line for Uno (change as needed)
SRAMsimple sram;       //initialize an instance of this class
byte data[128];
byte data2[128];

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);
    tritium_init();

    cctrl.lineOutGain(0, 0, false);

    for(uint i=0; i<sizeof(data); i++)
    {
    data[i] = i;
    }

    uint32_t address = 0;                       // create a 32 bit variable to hold the address (uint32_t=long)
    Serial.begin(9600);                         // set communication speed for the serial monitor
    SPI.begin();                                // start communicating with the memory chip

    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    int a = micros();

    sram.WriteByteArray(address, data, sizeof(data));

    int b = micros();

    sram.ReadByteArray(address, data2, sizeof(data2)); 

    int c = micros();

    SPI.endTransaction();
    Serial.print("Write time: ");
    Serial.println(b-a);

    Serial.print("Read time: ");
    Serial.println(c-b);

    Serial.println(data2[0]);
    Serial.println(data2[32]);
    Serial.println(data2[64]);
    Serial.println(data2[127]);
}

void loop()
{
    delay(500);
    // pinMode(P_SPI_CS, OUTPUT);
    // digitalWrite(P_SPI_CS, LOW);
}
