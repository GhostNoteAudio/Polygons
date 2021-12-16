#pragma once
#include <stdint.h>

namespace Polygons
{
    enum class ControlMode
    {
        None,
        //Paged,
        Encoded,
        Analog,
        Digital,
        DigitalToggle,
    };

    enum class MessageType
    {
        None,
        Digital,
        Encoder,
        Analog,
        Expression,
        ControlBoard,
    };

    enum class ControlBoard
    {
        Alpha = 1,
        Sigma = 2,
        Omega = 3
    };

    struct ParameterUpdate
    {
        MessageType Type;
        uint8_t Index;
        int16_t Value;

        ParameterUpdate()
        {
            Type = MessageType::None;
            Index = 0;
            Value = 0;
        }

        ParameterUpdate(MessageType type, uint8_t index, int16_t value)
        {
            Type = type;
            Index = index;
            Value = value;
        }
    };


}
