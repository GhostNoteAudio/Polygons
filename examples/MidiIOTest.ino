#include "blocks/DelayBlockExternal.h"
#include "SRAMsimple.h"
#include "Arduino.h"
#include "Tritium.h"

void setup()
{
    Serial.begin(115200);
    Serial1.begin(31250);
}

int lastUpdate = 0;
void loop()
{
    if (Serial1.available() >= 3) {
        byte data[3];
        Serial1.readBytes(data, 3);
        Serial.print("Message received: ");
        Serial.print(data[0]);
        Serial.print(", ");
        Serial.print(data[1]);
        Serial.print(", ");
        Serial.println(data[2]);
    }

    int time = millis();
    if (time - lastUpdate >= 500)
    {
        Serial.println("Banging data");
        byte data2[3] = { 0xB0, 0x01, 0x30};
        Serial1.write(data2, 3);
        lastUpdate = time;
    }
}
