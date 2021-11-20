#include <Base64.h>
#include "Arduino.h"
#include "Polygons.h"

namespace Polygons
{
    AudioControlTLV320AIC3204 codec;
    SRAMsimple sram;
    ControlMatrix controls;

    void (*ControlMatrix::onUpdate)(ControlType type, int index);
    GFXcanvas1 canvas(256, 64);
    char encodedString[2750];

    char ser_buffer[128];
    int ser_bufferIndex;

    ControlMatrix::ControlMatrix()
    {
        this->onUpdate = 0;
        this->Expression = 0;
        for (size_t i = 0; i < CONTROL_COUNT; i++)
        {
            this->Digital[i] = 0;
            this->Encoder[i] = 0;
            this->Analog[i] = 0;
            this->EncoderDelta[i] = 0;
            this->DigitalOut[i] = 0;
            this->AnalogOut[i] = 0;
        }
    }

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
        
        pinMode(P_EXP_IN, INPUT);
        pinMode(P_DAC_RESET, OUTPUT);
        
        // don't set SCL low, it screws with the I2C protocol
        // pinMode(P_SCL, OUTPUT);
    }

    void init()
    {
        Serial.begin(115200);
        Serial1.begin(115200);
        Serial3.begin(31250);

        controls.onUpdate = 0;

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
        codec.enableLoopbackAdc();
        Serial.println("Codec ready.");
    }

    GFXcanvas1* getCanvas()
    {
        return &canvas;
    }

    void ControlMatrix::pushDigital(uint8_t index, bool value)
    {
        Serial.print("$DO,");
        Serial.print(index);
        Serial.print(",");
        Serial.println(value);

        controls.DigitalOut[index] = value;
    }

    void ControlMatrix::pushAnalog(uint8_t index, uint16_t value)
    {
        Serial.print("$AO,");
        Serial.print(index);
        Serial.print(",");
        Serial.println(value);

        controls.AnalogOut[index] = value;
    }

    void ControlMatrix::readUpdates()
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
                ControlType type = (ControlType)(-1);

                if (strcmp(key, "$DI") == 0)
                {
                    controls.Digital[id] = value != 0;
                    type = ControlType::Digital;
                }
                else if (strcmp(key, "$EN") == 0)
                {
                    controls.EncoderDelta[id] = value;
                    controls.Encoder[id] += value;
                    type = ControlType::Encoder;
                }
                else if (strcmp(key, "$AN") == 0)
                {
                    controls.Analog[id] = value;
                    type = ControlType::Analog;
                }

                if (controls.onUpdate)
                    controls.onUpdate(type, id);
            }
        }
    }

    void pushDisplay()
    {
        auto buf = canvas.getBuffer();
        Base64.encode(encodedString, (char*)buf, 2048);
        Serial.print("$SC,");
        Serial.println(encodedString);
    }
}
