#pragma once

#include <Adafruit_GFX.h>
#include "Teensy4i2s.h"
#include "midiProcessing.h"
#include "SRAMsimple.h"

#define P_MIDI_RX 0
#define P_MIDI_TX 1
#define P_SPI_CS 10
#define P_SPI_DOUT 11
#define P_SPI_DIN 12
#define P_SPI_CLK 13
#define P_LATCH_ANALOG 14
#define P_LATCH_DIGITAL 15
#define P_SDA 18
#define P_SCL 19

#define I2C_ADDR_CODEC 0x18
#define I2C_ADDR_EEPROM 0x54

namespace Polygons
{
    enum class ControlType
    {
        Digital,
        Encoder,
        Analog,
        AnalogFast
    };

    const int CONTROL_COUNT = 64;

    struct ControlMatrix
    {
        bool Digital[CONTROL_COUNT]; // $DI
        int32_t Encoder[CONTROL_COUNT]; // $EN
        int8_t EncoderDelta[CONTROL_COUNT]; // $EN
        uint16_t Analog[CONTROL_COUNT]; // $AN
        uint16_t AnalogFast[CONTROL_COUNT]; // $AF

        bool DigitalOut[CONTROL_COUNT]; // $DO
        uint16_t AnalogOut[CONTROL_COUNT]; // $AO

        static void (*onUpdate)(ControlType type, int index);

        //  call these in the background loop
        static void pushDigital(uint8_t index, bool value); // $DO
        static void pushAnalog(uint8_t index, uint16_t value); // $AO

        // Processes data from the UART buffer and updates the control matrix with new values
        // call from audio processing loop
        static void readUpdates();

        ControlMatrix();
    };

    extern AudioControlTLV320AIC3204 cctrl;
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
