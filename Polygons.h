#pragma once

#define P_UART_RX 0 // Serial1
#define P_UART_TX 1
#define P_I2S_DOUT 7
#define P_I2S_DIN 8
#define P_SPI_SD_CS 9
#define P_SPI_SRAM_CS 10
#define P_SPI_DOUT 11
#define P_SPI_DIN 12
#define P_SPI_CLK 13
#define P_MIDI_TX 14 // Serial3
#define P_MIDI_RX 15
#define P_EXP_IN 16
#define P_DAC_RESET 17
#define P_SDA 18
#define P_SCL 19
#define P_I2S_LRCLK 20
#define P_I2S_BCLK 21
#define P_I2S_MCLK 23

#define I2C_ADDR_CODEC 0x18

#include <Adafruit_GFX.h>
#include "Teensy4i2s.h"
#include "midiProcessing.h"
#include "storage.h"
#include "SRAMsimple.h"

namespace Polygons
{
    enum class ControlType
    {
        Digital,
        Encoder,
        Analog,
        Expression,
    };

    const int CONTROL_COUNT = 64;

    struct ControlMatrix
    {
        bool Digital[CONTROL_COUNT]; // $DI
        int32_t Encoder[CONTROL_COUNT]; // $EN
        int8_t EncoderDelta[CONTROL_COUNT]; // $EN
        uint16_t Analog[CONTROL_COUNT]; // $AN
        uint16_t Expression; // $EX

        bool DigitalOut[CONTROL_COUNT]; // $DO
        uint16_t AnalogOut[CONTROL_COUNT]; // $AO

        static void (*onUpdate)(ControlType type, int index);

        //  call these in the background loop
        static void pushDigital(uint8_t index, bool value); // $DO
        static void pushAnalog(uint8_t index, uint16_t value); // $AO

        // Processes data from the UART buffer and updates the control matrix with new values
        // call from audio processing loop
        // Todo: Also read ADC on teensy for Expression input
        static void readUpdates();

        ControlMatrix();
    };

    extern AudioControlTLV320AIC3204 codec;
    extern SRAMsimple sram;
    extern ControlMatrix controls;

    // turns the RESET pin of the codec high
    void enableCodec();

    // Configures the INPUT/OUTPUT modes of pins
    void setPinModes();

    // Initialises the I2S bus, clock signals, and the codec
    void init();

    GFXcanvas1* getCanvas();

    void pushDisplay(); // $SC
}
