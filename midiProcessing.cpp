#include "midiProcessing.h"

void (*midiReceivedCallback)(byte type, byte channel, byte data1, byte data2, bool usbMidi) = 0;

void sendMidi(byte type, byte channel, byte data1, byte data2, bool usbMidi)
{
    if (usbMidi)
    {
        usbMIDI.send(type, data1, data2, channel, 0);   
    }
    else
    {
        byte data[3];
        data[0] = type | channel;
        data[1] = data1;
        data[2] = data2;
        Serial1.write(data, 3);
    }
}

void sendSysex(byte* data, int count, bool usbMidi)
{
    if (usbMidi)
    {
        usbMIDI.sendSysEx(count, data, true);
    }
    else
    {
        Serial1.write(data, count);
    }
}

byte s1Buffer[16];
byte s1BufPos = 0;
void readMidi()
{
    while (Serial1.available() > 0)
    {
        int value = Serial1.read();
        if (value >= 0x80)
        {
            s1Buffer[0] = value;
            s1BufPos = 1;
        }
        else if (s1BufPos < 3)
        {
            s1Buffer[s1BufPos] = value;
            s1BufPos++;
        }
        else
        {
            // this is probably a sysex message, todo: implement!
        }

        if (s1BufPos == 3)
        {
            byte type = 0xF0 & s1Buffer[0];
            byte channel = 0x0F & s1Buffer[0];
            midiReceivedCallback(type, channel, s1Buffer[1], s1Buffer[2], false);
        }
    }

    if (usbMIDI.read())
    {
        byte type = usbMIDI.getType();
        byte channel = usbMIDI.getChannel();
        byte data1 = usbMIDI.getData1();
        byte data2 = usbMIDI.getData2();
        if (midiReceivedCallback)
            midiReceivedCallback(type, channel, data1, data2, true);
    }
}