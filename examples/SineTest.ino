#include "Arduino.h"
#include "Polygons.h"

double gain = 0.0;
int freq = 400;
double wave = 0.0;
int n = 0;
int maxScaler = SAMPLE_32_MAX / 2;
double minInput = 0.0;
double maxInput = 0.0;

double lpf = 0.0;

void audioCallbackMute(int32_t** inputs, int32_t** outputs)
{
    double scaler = 1.0 / SAMPLE_32_MAX;
    for (size_t i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
    {
        double input = inputs[0][i] * scaler;

        lpf = lpf * 0.999 + input * (1-0.999);
        input = input - lpf;

        input += sin(n++ * freq / (double)SAMPLERATE * 2.0 * M_PI) * wave;
        if (gain != 0.0f)
        {
            input = input*gain;
        }

        if (input < minInput)
            minInput = input;
        if (input > maxInput)
            maxInput = input;

        if (input > 1)
            input = 1;
        if (input < -1)
            input = -1;

        outputs[0][i] = input * maxScaler;
        outputs[1][i] = input * maxScaler;
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(LED_BUILTIN,OUTPUT);
    Polygons::init();

    //cctrl.enableLoopbackAdc();
    //Serial.println("loopback enabled.");

    //cctrl.dacVolume(-20, -20);
    //cctrl.adcGain(-24, -24);
    Polygons::codec.lineOutGain(0, 0, false);

    i2sAudioCallback = audioCallbackMute;
}

int32_t lastUpdate = 0;

char data[64];
int dataPtr = 0;

void processMessage()
{
    Serial.print("Incoming message: ");
    Serial.println(data);

    auto key = strtok (data,",");
    auto val = strtok (NULL, ",");
    char **ptr = 0;
    auto value = strtol(val, ptr, 10);

    if (strcmp(key, "gain")==0)
    {
        gain = value / 100.0;
        Serial.print("Gain:");
        Serial.println(gain);
    }
    if (strcmp(key, "freq")==0)
    {
        freq = value;
        Serial.print("Freq:");
        Serial.println(freq);
    }
    if (strcmp(key, "wave")==0)
    {
        wave = value / 100.0;
        Serial.print("Wave:");
        Serial.println(wave);
    }
    if (strcmp(key, "adc")==0)
    {
        Serial.print("ADC Gain:");
        Serial.println(value);
        Polygons::codec.analogInGain(value, value);
    }
}

void readInput()
{
    if(Serial.available() == 0)
        return;
    char d = Serial.read();
    data[dataPtr] = d;
    
    if (d == '\n')
    {
        data[dataPtr] = 0;
        dataPtr = 0;
        processMessage();
    }
    else
    {
        dataPtr++;
    }
}

void loop()
{
    auto time = millis();
    if (time - lastUpdate > 500)
    {
        lastUpdate = time;
        digitalWriteFast(LED_BUILTIN,!digitalReadFast(LED_BUILTIN));
        Serial.print("Input Range: ");
        Serial.print(minInput, 10);
        Serial.print(" :: ");
        Serial.println(maxInput, 10);
        minInput = 999999;
        maxInput = -999999;
    }

    readInput();
}
