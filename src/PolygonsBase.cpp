#include <string.h>
#include "PolygonsBase.h"
#include "storage.h"
#include "Logging.h"
#include "HexCompression.h"

namespace Polygons
{
    Stream* SerialControl;
    AudioControlTLV320AIC3204 codec;
    SRAMsimple sram;
    GFXcanvas1 canvas256(256, 64);
    GFXcanvas1 canvas128(128, 64);
    bool useLargeDisplay;

    // This buffers incoming string commands
    const int SerialBufferSize = 128;
    char serialBuffer[SerialBufferSize];
    int serialBufferIndex;

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
        SerialControl = &Serial; // default to USB serial control

        Serial.begin(115200);
        Serial1.begin(250000);
        Serial3.begin(31250);

        LogInfo("Setting pin modes...")
        setPinModes();

        LogInfo("Enabling codec...")
        enableCodec();    

        LogInfo("Setting up I2S audio and clocks...")
        InitI2s();

        LogInfo("starting codec stuff...")
        if (codec.testConnection())
        {
            LogInfo("Codec is active")
        }
        else
        {
            LogInfo("Codec is unresponsive!")
        }
        

        codec.init();

        LogInfo("Setting initial gain...")
        codec.analogInGain(0, 0);
        codec.headphoneGain(0, 0, false);
        codec.lineOutGain(0, 0, false);
        //codec.enableLoopbackAdc();
        LogInfo("Codec ready.")

        Storage::InitStorage();
        setBypass(0);
    }

    void pushDigital(uint8_t index, bool value)
    {
        SerialControl->print("$DO,");
        SerialControl->print(index);
        SerialControl->print(",");
        SerialControl->println(value);
        delayMicroseconds(20);
    }

    void pushAnalog(uint8_t index, uint16_t value)
    {
        SerialControl->print("$AO,");
        SerialControl->print(index);
        SerialControl->print(",");
        SerialControl->println(value);
        delayMicroseconds(20);
    }

    bool CheckMessageType(const char* data, const char* msgType)
    {
        auto x0 = data[0] == msgType[0];
        auto x1 = data[1] == msgType[1];
        auto x2 = data[2] == msgType[2];
        return x0 && x1 && x2;
    }

    int ParseValue(const char* data, int offset = 0)
    {
        char temp[12] = { 0 };
        int startIdx = -1;
        int endIdx = -1;
        int commasToSkip = offset + 1;

        int idx = 3;
        for (idx = 3; idx < 128; idx++)
        {
            if (data[idx] == 0 || data[idx] == '\n')
            {
                endIdx = idx - 1;
                break;
            }

            if (data[idx] == ',')
                commasToSkip--;

            if (commasToSkip <= 0 && data[idx] == ',')
            {
                if (startIdx == -1)
                    startIdx = idx + 1;
                else
                    endIdx = idx - 1;
            }

            if (startIdx != -1 && endIdx != -1)
                break;
        }

        if (startIdx == -1 || endIdx == -1)
            return 0;

        if (endIdx - startIdx >= 12)
            return 0;

        memcpy(temp, &data[startIdx], endIdx - startIdx + 1);
        return atol(temp);
    }

    ParameterUpdate getUpdate(Stream* serialControl)
    {
        while (serialControl->available() > 0)
        {
            char val = serialControl->read();
            serialBuffer[serialBufferIndex] = val;
            serialBufferIndex++;
            if (serialBufferIndex == SerialBufferSize)
            {
                LogWarn("Serial buffer overflow, resetting!")
                serialBufferIndex = 0;
                return ParameterUpdate();
            }

            if (val == '\n')
            {
                serialBuffer[serialBufferIndex-1] = 0;
                if (serialBuffer[serialBufferIndex-2] == '\r')
                    serialBuffer[serialBufferIndex-2] = 0; // remove the carriage return as well, if sent
                
                serialBufferIndex = 0;
                //Serial.print("Received command: ");
                //Serial.println(serialBuffer);

                auto type = MessageType::None;
                int id = 0;
                int value = 0;

                if (CheckMessageType(serialBuffer, "$DI"))
                {
                    type = MessageType::Digital;
                    id = ParseValue(serialBuffer, 0);
                    value = ParseValue(serialBuffer, 1);
                }
                else if (CheckMessageType(serialBuffer, "$EN"))
                {
                    type = MessageType::Encoder;
                    id = ParseValue(serialBuffer, 0);
                    value = ParseValue(serialBuffer, 1);
                }
                else if (CheckMessageType(serialBuffer, "$AN"))
                {
                    type = MessageType::Analog;
                    id = ParseValue(serialBuffer, 0);
                    value = ParseValue(serialBuffer, 1);
                }
                else if (CheckMessageType(serialBuffer, "$CB"))
                {
                    type = MessageType::ControlBoard;
                    id = ParseValue(serialBuffer, 0);
                }
                else if (CheckMessageType(serialBuffer, "$LG"))
                {
                    type = MessageType::ControlBoard;
                    LogInfof("LOG: %s", &serialBuffer[4]);
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

    // Call this function to push the display buffer out to serial port.
    // Each calls will send a certain number of bytes. You must continue to call
    // this until it returns True, indicating that all bytes in the buffer have been sent.
    // increment the updateCycle by 1 each time, until you get a true return, then start
    // again at zero.
    bool pushDisplay(int updateCycle)
    {
        char txBuf[512];
        auto canvas = getCanvas();
        int dataSize = (canvas->width() * canvas->height()) / 8;
        auto buf = canvas->getBuffer();

        int chunkSize = 256;
        int bytesRemaining = dataSize - chunkSize * updateCycle;
        int bytesToSend = bytesRemaining > chunkSize ? chunkSize : bytesRemaining;

        bool completed = false;
        SerialControl->print("$SC,");
        SerialControl->print(updateCycle * chunkSize);
        SerialControl->print(",");

        if (bytesToSend <= 0)
            completed = true;
        else if (bytesRemaining == chunkSize)
            completed = true;
        else
            completed = false;

        int strLen = Compress(&buf[chunkSize * updateCycle], bytesToSend, txBuf);
        SerialControl->write(txBuf, strLen);
        SerialControl->println("");
        delayMicroseconds(20);

        return completed;
    }

    // Pushes the full display buffer, this can take a while so it might block
    void pushDisplayFull()
    {
        int cycle = 0;
        while(true)
        {
            Serial.println("Pushing...");
            bool res = pushDisplay(cycle);
            if (res)
                break;
            cycle++;
        }
    }
}
