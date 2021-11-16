#include <Base64.h>
#include "Arduino.h"
#include "Tritium.h"

AudioControlTLV320AIC3204 cctrl;
SRAMsimple sram;
ControlMatrix controls;

void (*ControlMatrix::onUpdate)(ControlType type, int index);
GFXcanvas1 canvas(128, 64);
char encodedString[1400];

char ser_buffer[128];
int ser_bufferIndex;

ControlMatrix::ControlMatrix()
{
    this->onUpdate = 0;
    for (size_t i = 0; i < CONTROL_COUNT; i++)
    {
        this->Digital[i] = 0;
        this->Encoder[i] = 0;
        this->Analog[i] = 0;
        this->AnalogFast[i] = 0;
        this->EncoderDelta[i] = 0;
        this->DigitalOut[i] = 0;
        this->AnalogOut[i] = 0;
    }
}

void enableCodec()
{ 
    digitalWrite(P_SPI_DOUT, HIGH);
    digitalWrite(P_SPI_CLK, LOW);
    digitalWrite(P_LATCH_ANALOG, LOW);
    delay(1);
    for(int i=0; i<8; i++)
    {
        digitalWrite(P_SPI_CLK, HIGH);
        delay(1);
        digitalWrite(P_SPI_CLK, LOW);
        delay(1);
    }

    digitalWrite(P_LATCH_ANALOG, HIGH);
    delay(1);
    digitalWrite(P_LATCH_ANALOG, LOW);
    delay(1);  
}

// Sets all the necessary input and output pin modes
void setPinModes()
{
    pinMode(P_SPI_CS, OUTPUT);
    pinMode(P_SPI_DOUT, OUTPUT);
    pinMode(P_SPI_DIN, INPUT);
    pinMode(P_SPI_CLK, OUTPUT);

    //pinMode(P_MIDI_RX, INPUT);
    //pinMode(P_MIDI_TX, OUTPUT);
    
    pinMode(P_LATCH_ANALOG, OUTPUT);
    pinMode(P_LATCH_DIGITAL, OUTPUT);
    
    //pinMode(P_SCL, OUTPUT); // don't set SCL low, it fucks with the protocol
}

void tritiumInit()
{
    Serial.begin(115200);
    Serial1.begin(31250);
    controls.onUpdate = 0;

    Serial.println("Setting pin modes...");
    setPinModes();

    Serial.println("Enabling codec...");
    enableCodec();    

    Serial.println("Setting up I2S audio and clocks...");
    InitI2s();

    Serial.println("starting codec stuff...");
    cctrl.init();

    Serial.println("Setting initial gain...");
    cctrl.analogInGain(0, 0);
    cctrl.headphoneGain(0, 0, false);
    cctrl.lineOutGain(0, 0, false);
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
            else if (strcmp(key, "$AF") == 0)
            {
                controls.AnalogFast[id] = value;
                type = ControlType::AnalogFast;
            }

            if (controls.onUpdate)
                controls.onUpdate(type, id);
        }
    }
}

void pushDisplay()
{
    auto buf = canvas.getBuffer();
    Base64.encode(encodedString, (char*)buf, 1024);
    Serial.print("$SC,");
    Serial.println(encodedString);
}
