#pragma once
#include <stdint.h>
#include <Adafruit_GFX.h>
#include "Types.h"
#include "menuManager.h"
#include "Utils.h"
#include "PolygonsBase.h"

namespace Polygons
{
    struct ParameterRegistration
    {
        uint8_t ParamId;
        uint16_t MaxValue;        
        ControlMode Mode;
        uint8_t Index;
        uint16_t EncoderDelta;
        uint16_t Value;
    };

    class PolyOS
    {
    public:
        MenuManager menu;
        ControlBoard controlBoard;
        ParameterRegistration Parameters[256];
        
        bool (*HandleUpdateCallback)(ParameterUpdate* update);
        void (*SetParameterCallback)(uint8_t paramId, uint16_t value);
        uint8_t PageCount;
        int SelectedPage;

        PolyOS() : menu(this)
        {
            for (size_t i = 0; i < 256; i++)
            {
                Parameters[i].Mode = ControlMode::None;
                Parameters[i].Index = 0;
                Parameters[i].EncoderDelta = 1;
                Parameters[i].MaxValue = 1023;
                Parameters[i].ParamId = 0;
                Parameters[i].Value = 0;
            }
            
            HandleUpdateCallback = 0;
            SetParameterCallback = 0;
            controlBoard = ControlBoard::Alpha;
        }

        void Register(
            uint8_t paramId, 
            uint16_t maxValue, 
            ControlMode mode,
            uint8_t index,
            uint16_t encoderDelta)
        {
            int idx = paramId;
            Parameters[idx].ParamId = paramId;
            Parameters[idx].MaxValue = maxValue;
            Parameters[idx].Mode = mode;
            Parameters[idx].Index = index;
            Parameters[idx].EncoderDelta = encoderDelta;
        }

        /*int getParamPage(int page, int pagePos)
        {
            for (size_t i = 0; i < 256; i++)
            {
                if (Parameters[i].Mode == ControlMode::Paged && Parameters[i].Index == page * 8 + pagePos)
                    return i;
            }

            return -1;
        }*/

        int getParamEncoded(int absIndex)
        {
            for (size_t i = 0; i < 256; i++)
            {
                if (Parameters[i].Mode == ControlMode::Encoded && Parameters[i].Index == absIndex)
                    return i;
            }

            return -1;
        }

        int getParamDigital(int index)
        {
            for (size_t i = 0; i < 256; i++)
            {
                if ((Parameters[i].Mode == ControlMode::Digital || Parameters[i].Mode == ControlMode::DigitalToggle) && Parameters[i].Index == index)
                    return i;
            }

            return -1;
        }

        void HandleUpdate(MessageType type, int index, int value)
        {
            if (type == MessageType::Digital && index < PageCount)
            {
                if (controlBoard == ControlBoard::Alpha)
                {
                    SelectedPage = index;
                    menu.setUpdated();
                }
                else if (controlBoard == ControlBoard::Sigma)
                {
                    if (index == 0)
                    {
                        value = value > 0 ? 0b01 : 0;
                        SelectedPage = (SelectedPage & 0b10) | value;
                    }
                    else if (index == 1)
                    {
                        value = value > 0 ? 0b10 : 0;
                        SelectedPage = (SelectedPage & 0b01) | value;
                    }
                }
            }
            else if (type == MessageType::Encoder)
            {
                int paramId = -1;
                paramId = getParamEncoded(SelectedPage * 8 + index);
                
                if (paramId >= 0)
                {
                    int newVal = Parameters[paramId].Value + Parameters[paramId].EncoderDelta * value;
                    newVal = Clip(newVal, 0, Parameters[paramId].MaxValue);
                    Parameters[paramId].Value = newVal;
                    if (SetParameterCallback != 0)
                        SetParameterCallback(paramId, newVal);
                    menu.setUpdated(index);
                }
            }
            else if (type == MessageType::Digital)
            {
                int paramId = -1;
                paramId = getParamDigital(index);
                bool toggle = Parameters[paramId].Mode == ControlMode::DigitalToggle;

                if (paramId >= 0)
                {
                    int newVal = 0;
                    if (toggle)
                    {
                        if (value > 0)
                            newVal = Parameters[paramId].Value == 0 ? Parameters[paramId].MaxValue : 0;
                        else
                            return;
                    }
                    else
                    {
                        newVal = value == 0 ? 0 : Parameters[paramId].MaxValue;
                    }
                    Parameters[paramId].Value = newVal;

                    if (SetParameterCallback != 0)
                        SetParameterCallback(paramId, newVal);
                }
            }
        }

        Polygons::MenuManager* getMenu()
        {
            return &menu;
        }

        void waitForControllerSignal()
        {
            while(true)
            {
                Serial.println("Waiting for Control Board to send announcement...");
                auto update = Polygons::getUpdate();
                if (update.Type != MessageType::ControlBoard)
                    delay(200);
                else
                {
                    this->controlBoard = (ControlBoard)update.Index;
                    useLargeDisplay = this->controlBoard == ControlBoard::Alpha;
                    break;
                }
            }
            Serial.print("Received message from Control Board: ");
            Serial.println((int)controlBoard);
        }

        void loop()
        {
            //int expVal = getAnalogFast();
            
            while(true)
            {
                auto update = Polygons::getUpdate();
                if (update.Type == MessageType::None)
                    break;
                bool handled = false;
                if (HandleUpdateCallback != 0)
                    handled = HandleUpdateCallback(&update);
                if (!handled)
                    HandleUpdate(update.Type, update.Index, update.Value);
            }

            if (controlBoard == ControlBoard::Alpha)
                MenuManagerDrawing::DrawAlphaMenu(getMenu());
            else if (controlBoard == ControlBoard::Sigma)
                MenuManagerDrawing::DrawSigmaMenu(getMenu());
            // else // no menu to be drawn, control board might not have a display

            Polygons::pushDisplay();
        }
    };
}