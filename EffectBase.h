#pragma once

#include "Polygons.h"

namespace Polygons
{
    bool xHandleUpdate(Polygons::ParameterUpdate* update);

    class EffectBase
    {
        static EffectBase* Instance;
    public:
        Polygons::PolyOS os;

        virtual void RegisterParams() = 0;
        virtual void GetPageName(int page, char* dest) = 0;
        virtual void GetParameterName(int paramId, char* dest) = 0;
        virtual void GetParameterDisplay(int paramId, char* dest) = 0;
        virtual void SetParameter(uint8_t paramId, uint16_t value) = 0;
        virtual bool HandleUpdate(Polygons::ParameterUpdate* update) { return false; }
        virtual void CustomDrawCallback() { };
        virtual void AudioCallback(int32_t** inputs, int32_t** outputs, int bufferSize) = 0;
        virtual void Start() = 0;
        virtual void Loop() { os.loop(); }

    private:
        static bool xHandleUpdate(Polygons::ParameterUpdate* update)
        { return Instance->HandleUpdate(update); }

        static void xSetParameter(uint8_t paramId, uint16_t value)
        { Instance->SetParameter(paramId, value); }

        static void xGetPageName(int page, char* dest)
        { Instance->GetPageName(page, dest); }

        static void xGetParameterName(int paramId, char* dest)
        { Instance->GetParameterName(paramId, dest); }

        static void xGetParameterDisplay(int paramId, char* dest)
        { Instance->GetParameterDisplay(paramId, dest); }

        static void xAudioCallback(int32_t** inputs, int32_t** outputs)
        { Instance->AudioCallback(inputs, outputs, AUDIO_BLOCK_SAMPLES); }

        static void xCustomDrawCallback()
        { Instance->CustomDrawCallback(); }

    public:
        virtual void RegisterEffect()
        {
            Instance = this;
            RegisterParams();

            os.HandleUpdateCallback = xHandleUpdate;
            os.SetParameterCallback = xSetParameter;
            os.menu.getPageName = xGetPageName;
            os.menu.getParameterName = xGetParameterName;
            os.menu.getParameterDisplay = xGetParameterDisplay;
            os.CustomDrawCallback = xCustomDrawCallback;
            i2sAudioCallback = xAudioCallback;
        }
    };

    
}