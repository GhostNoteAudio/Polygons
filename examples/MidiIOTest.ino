#include "SRAMsimple.h"
#include "Arduino.h"
#include "Polygons.h"

using namespace Polygons::Midi;
int lastUpdate = 0;

void handleMidi(byte type, byte channel, byte data1, byte data2, bool usbMidi)
{
    Serial.print("Midi message: 0x");
    Serial.print(type, 16);
    Serial.print(", Channel: ");
    Serial.print(channel);
    Serial.print(", Data: ");
    Serial.print(data1, 16);
    Serial.print(" ");
    Serial.print(data2, 16);
    Serial.print(" - USB Midi: ");
    Serial.println(usbMidi);
}

void setup()
{
    Polygons::init();
}

void loop()
{
    midiReceivedCallback = handleMidi;
    readMidi();

    int time = millis();
    if (time - lastUpdate >= 500)
    {
        Serial.println("Sending some midi data to both USB and hardware ports...");
        sendMidi(0xB0, 1, 2, 3, false);
        sendMidi(0xB0, 2, 4, 5, true);
        lastUpdate = time;
    }
}
