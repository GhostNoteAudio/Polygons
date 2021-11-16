#include "Arduino.h"
#include "Tritium.h"

void audioCallbackMute(int32_t** inputs, int32_t** outputs)
{
    readMidi();
}

void handleMidi(byte type, byte channel, byte data1, byte data2, bool usbMidi)
{
    Serial.print("received midi: 0x");
    Serial.print(type, 16);
    Serial.print(" 0x");
    Serial.print(channel, 16);
    Serial.print(" 0x");
    Serial.print(data1, 16);
    Serial.print(" 0x");
    Serial.print(data2, 16);
    Serial.print(" - USB: ");
    Serial.println(usbMidi);
}

void setup()
{
    tritium_init();
    
    cctrl.lineOutGain(0, 0, false);
    midiReceivedCallback = handleMidi;
    i2sAudioCallback = audioCallbackMute;
}

void loop()
{

}
