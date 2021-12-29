#include "Buffers.h"
#include "Arduino.h"
#include "AudioConfig.h"

namespace Polygons
{
    namespace Buffers
    {
        float BufferData[BUFFER_COUNT * AUDIO_BLOCK_SAMPLES];
        bool Taken[BUFFER_COUNT] = {false};

        Buffer Request()
        {
            for (int i = 0; i < BUFFER_COUNT; i++)
            {
                if (!Taken[i])
                {
                    Taken[i] = true;
                    return Buffer(i, &BufferData[i*AUDIO_BLOCK_SAMPLES]);
                }
            }

            return Buffer(0, 0);
        }
    }

    Buffer::Buffer(int idx, float* ptr)
    {
        Idx = idx;
        Ptr = ptr;
    }

    Buffer::~Buffer()
    {
        Buffers::Taken[Idx] = false;
        Idx = 9999;
    }
}
