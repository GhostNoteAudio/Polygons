#define BUFFER_COUNT 16

namespace Polygons
{
    class Buffer;

    class Buffer
    {
    public:
        int Idx;
        float* Ptr;
        Buffer(int idx, float* ptr);
        ~Buffer();
    };

    namespace Buffers
    {
        Buffer Request();
    }
}
