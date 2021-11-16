#include "blocks/DelayBlockExternal.h"
#include "SRAMsimple.h"
#include "Arduino.h"
#include "Tritium.h"

DMAMEM DelayBlockExternal <32768, AUDIO_BLOCK_SAMPLES> d1(0);
int32_t buffer[AUDIO_BLOCK_SAMPLES] = {0};
void audioCallback(int32_t** inputs, int32_t** outputs)
{
    SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
    d1.read(buffer, 25600, AUDIO_BLOCK_SAMPLES);

    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        buffer[i] = inputs[0][i] + 0.8 * buffer[i];
    }

    d1.write(buffer, AUDIO_BLOCK_SAMPLES);
    d1.updatePtr(AUDIO_BLOCK_SAMPLES);
    
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        outputs[0][i] = outputs[1][i] = buffer[i];
    }

    SPI.endTransaction();
}

void setup()
{
    Serial.begin(115200);
    tritium_init();

    
    d1.init();

    i2sAudioCallback = audioCallback;
}

void loop()
{
    delay(500);
    Serial.print("CPU Load: ");
    Serial.println(Timers::GetCpuLoad() * 100);
}
