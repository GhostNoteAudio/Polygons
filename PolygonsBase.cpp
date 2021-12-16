#include "PolygonsBase.h"
#include "storage.h"
#include <Base64.h>

namespace Polygons
{
    AudioControlTLV320AIC3204 codec;
    SRAMsimple sram;
    GFXcanvas1 canvas256(256, 64);
    GFXcanvas1 canvas128(128, 64);
    bool useLargeDisplay;

    // big enough to encode 256*64 canvas
    char encodedString[2750];

    char ser_buffer[128];
    int ser_bufferIndex;

    void enableCodec()
    { 
        digitalWrite(P_DAC_RESET, HIGH);
        delay(1);
    }

    // Sets all the necessary input and output pin modes
    void setPinModes()
    {
        pinMode(P_SPI_SD_CS, OUTPUT);
        pinMode(P_SPI_SRAM_CS, OUTPUT);
        pinMode(P_SPI_DOUT, OUTPUT);
        pinMode(P_SPI_DIN, INPUT);
        pinMode(P_SPI_CLK, OUTPUT);

        // Serial.begin takes care of these
        // pinMode(P_UART_RX, INPUT);
        // pinMode(P_UART_TX, OUTPUT);
        // pinMode(P_MIDI_RX, INPUT);
        // pinMode(P_MIDI_TX, OUTPUT);
        
        pinMode(P_FX_BYPASS, OUTPUT);
        pinMode(P_EXP_IN, INPUT);
        pinMode(P_DAC_RESET, OUTPUT);
        
        // don't set SCL low, it screws with the I2C protocol
        // pinMode(P_SCL, OUTPUT);
    }

    void init()
    {
        useLargeDisplay = true;

        Serial.begin(115200);
        Serial1.begin(115200);
        Serial3.begin(31250);

        Serial.println("Setting pin modes...");
        setPinModes();

        Serial.println("Enabling codec...");
        enableCodec();    

        Serial.println("Setting up I2S audio and clocks...");
        InitI2s();

        Serial.println("starting codec stuff...");
        if (codec.testConnection())
            Serial.println("Codec is active");
        else
            Serial.println("Codec is unresponsive!");

        codec.init();

        Serial.println("Setting initial gain...");
        codec.analogInGain(0, 0);
        codec.headphoneGain(0, 0, false);
        codec.lineOutGain(0, 0, false);
        //codec.enableLoopbackAdc();
        Serial.println("Codec ready.");

        Storage::InitStorage();
    }

    void pushDigital(uint8_t index, bool value)
    {
        Serial.print("$DO,");
        Serial.print(index);
        Serial.print(",");
        Serial.println(value);
    }

    void pushAnalog(uint8_t index, uint16_t value)
    {
        Serial.print("$AO,");
        Serial.print(index);
        Serial.print(",");
        Serial.println(value);
    }

    ParameterUpdate getUpdate()
    {
        while (Serial.available() > 0)
        {
            char val = Serial.read();
            ser_buffer[ser_bufferIndex] = val;
            ser_bufferIndex++;
            if (val == '\n')
            {
                ser_buffer[ser_bufferIndex-1] = 0;
                ser_bufferIndex = 0;
                char **ptr = 0;

                auto key = strtok (ser_buffer, ",");
                auto val = strtok (NULL, ",");
                int id = strtol(val, ptr, 10);
                val = strtok (NULL, ",");
                int value = strtol(val, ptr, 10);
                MessageType type = MessageType::None;

                if (strcmp(key, "$DI") == 0)
                {
                    type = MessageType::Digital;
                }
                else if (strcmp(key, "$EN") == 0)
                {
                    type = MessageType::Encoder;
                }
                else if (strcmp(key, "$AN") == 0)
                {
                    type = MessageType::Analog;
                }
                else if (strcmp(key, "$CB") == 0)
                {
                    type = MessageType::ControlBoard;
                }

                return ParameterUpdate(type, id, value);
            }
        }

        return ParameterUpdate();
    }

    int getAnalogFast()
    {
        int expValue = analogRead(2);
        return expValue;
    }

    void setBypass(bool bypass)
    {
        digitalWrite(P_FX_BYPASS, bypass ? 1 : 0);
    }

    GFXcanvas1* getCanvas()
    {
        return useLargeDisplay ? &canvas256 : &canvas128;
    }

    void pushDisplay()
    {
        auto canvas = getCanvas();
        int dataSize = (canvas->width() * canvas->height()) / 8;
        auto buf = canvas->getBuffer();
        Base64.encode(encodedString, (char*)buf, dataSize);
        Serial.print("$SC,");
        Serial.println(encodedString);
    }
}
