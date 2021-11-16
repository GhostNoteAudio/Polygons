#include "blocks/DelayBlock.h"
#include "SRAMsimple.h"
#include "Arduino.h"
#include "Tritium.h"


#define CSPIN 10       // Default Chip Select Line for Uno (change as needed)
SRAMsimple sram;       //initialize an instance of this class

DMAMEM DelayBlock <32768> d1(0);

void audioCallback(int32_t** inputs, int32_t** outputs)
{
    float buffer[AUDIO_BLOCK_SAMPLES];
    
    d1.write(inputs[0], AUDIO_BLOCK_SAMPLES);
    d1.read(buffer, 24000, AUDIO_BLOCK_SAMPLES);
    d1.writeAdd(buffer, AUDIO_BLOCK_SAMPLES, 0.8f);
    d1.updatePtr(AUDIO_BLOCK_SAMPLES);
    
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        outputs[0][i] = outputs[1][i] = inputs[0][i] + buffer[i] * SAMPLE_32_MAX;
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);
    tritium_init();

    cctrl.lineOutGain(0, 0, false);

    i2sAudioCallback = audioCallback;
}

void loop()
{
    delay(500);
    Serial.print("CPU Load: ");
    Serial.println(Timers::GetCpuLoad() * 100);
}
