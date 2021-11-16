#pragma once

#include "Arduino.h"
#include <stdint.h>

namespace Polygons
{
    extern void (*midiReceivedCallback)(byte type, byte channel, byte data1, byte data2, bool usbMidi);
    void sendMidi(byte type, byte channel, byte data1, byte data2, bool usbMidi);
    void sendSysex(byte* data, int count, bool usbMidi);
    void readMidi();
}
