#include "midiProcessing.h"

namespace Polygons
{
    namespace Midi
    {
        // Note: Channel will be 1-16
        void (*midiReceivedCallback)(byte type, byte channel, byte data1, byte data2, bool usbMidi) = 0;

        // Note: Channel should be 1-16
        void sendMidi(byte type, byte channel, byte data1, byte data2, bool usbMidi)
        {
            if (usbMidi)
            {
                usbMIDI.send(type, data1, data2, channel, 0);   
            }
            else
            {
                byte data[3];
                data[0] = type | (channel-1);
                data[1] = data1;
                data[2] = data2;
                Serial3.write(data, 3);
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
                Serial3.write(data, count);
            }
        }

        byte s3Buffer[16];
        byte s3BufPos = 0;
        void readMidi()
        {
            while (Serial3.available() > 0)
            {
                int value = Serial3.read();
                if (value >= 0x80)
                {
                    s3Buffer[0] = value;
                    s3BufPos = 1;
                }
                else if (s3BufPos < 3)
                {
                    s3Buffer[s3BufPos] = value;
                    s3BufPos++;
                }
                else
                {
                    // this is probably a sysex message, todo: implement!
                }

                if (s3BufPos == 3)
                {
                    byte type = 0xF0 & s3Buffer[0];
                    byte channel = 0x0F & s3Buffer[0];
                    midiReceivedCallback(type, channel + 1, s3Buffer[1], s3Buffer[2], false);
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
    }    
}
