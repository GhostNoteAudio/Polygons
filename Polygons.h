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
#include "Types.h"
#include "SRAMsimple.h"
#include "storage.h"
#include "midiProcessing.h"
#include "PolygonsBase.h"
#include "PolyOS.h"
#include "Buffers.h"