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
        int displayUpdateCycle = 0;
        
        bool (*HandleUpdateCallback)(ParameterUpdate* update);
        void (*SetParameterCallback)(uint8_t paramId, uint16_t value);
        void (*CustomDrawCallback)();
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
            CustomDrawCallback = 0;
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
                if ((Parameters[i].Mode == ControlMode::Digital 
                    || Parameters[i].Mode == ControlMode::DigitalToggle
                    || Parameters[i].Mode == ControlMode::DigitalUpToggle) 
                && Parameters[i].Index == index)
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
                    newVal = ClipI(newVal, 0, Parameters[paramId].MaxValue);
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
                Serial.print("paramId: ");
                Serial.println(paramId);
                Serial.print("paramMode: ");
                Serial.println((int)Parameters[paramId].Mode);

                bool toggle = Parameters[paramId].Mode == ControlMode::DigitalToggle || Parameters[paramId].Mode == ControlMode::DigitalUpToggle;
                bool onUp = Parameters[paramId].Mode == ControlMode::DigitalUpToggle;
                bool onDown = !onUp;

                if (paramId >= 0)
                {
                    int newVal = 0;
                    if (toggle)
                    {
                        bool shouldToggle = ((value > 0) && onDown) || ((value == 0) && onUp);
                        if (shouldToggle)
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
            else if (type == MessageType::ControlBoard)
            {
                // control board will occasionally re-send its identifier.
                // if the identifier matches the currently selected control board, respond with an ack
                if (index == (int)controlBoard)
                {
                    SerialControl->println("$CA,1");
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
                Serial.println("Scanning hardware and USB serial ports for Control Board announcement...");

                auto updateUsb = Polygons::getUpdate(&Serial);
                auto updateHw = Polygons::getUpdate(&Serial1);
                if (updateUsb.Type == MessageType::ControlBoard)
                {
                    this->controlBoard = (ControlBoard)updateUsb.Index;
                    useLargeDisplay = this->controlBoard == ControlBoard::Alpha;
                    SerialControl = &Serial;
                    Serial.println("Using USB Serial for control");
                    break;
                }
                else if (updateHw.Type == MessageType::ControlBoard)
                {
                    this->controlBoard = (ControlBoard)updateHw.Index;
                    useLargeDisplay = this->controlBoard == ControlBoard::Alpha;
                    SerialControl = &Serial1;
                    Serial.println("Using Hardware Serial for control");
                    break;
                }
                else
                {
                    delay(500);
                }
            }

            Serial.print("Received Control Board ID: ");
            Serial.println((int)controlBoard);
            SerialControl->println("$CA,1");
        }

        void loop()
        {
            //int expVal = getAnalogFast();
            while(true)
            {
                auto update = Polygons::getUpdate(SerialControl);
                if (update.Type == MessageType::None)
                    break;
                bool handled = false;
                if (HandleUpdateCallback != 0)
                    handled = HandleUpdateCallback(&update);
                if (!handled)
                    HandleUpdate(update.Type, update.Index, update.Value);
            }

            if (displayUpdateCycle == 0)
            {
                if (controlBoard == ControlBoard::Alpha)
                    MenuManagerDrawing::DrawAlphaMenu(getMenu());
                else if (controlBoard == ControlBoard::Sigma)
                    MenuManagerDrawing::DrawSigmaMenu(getMenu());
                // else // no menu to be drawn, control board might not have a display
                
                if (CustomDrawCallback != 0)
                    CustomDrawCallback();
            }
          
            for (size_t i = 0; i < 4; i++)
            {
                bool completed = Polygons::pushDisplay(displayUpdateCycle);
                if (completed)
                {
                    displayUpdateCycle = 0;
                    break;
                }
                else
                {
                    displayUpdateCycle++;
                }
            }
        }
    };
}