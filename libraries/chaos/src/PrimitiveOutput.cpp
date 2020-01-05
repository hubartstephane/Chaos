#include <chaos/PrimitiveOutput.h>

namespace chaos
{

    char* PrimitiveOutputBase::GeneratePrimitive(size_t required_size)
    {
        assert(required_size > 0);

        // enought memory reserved for this primitive
        if (buffer_start + required_size <= buffer_end)
        {
            char* result = buffer_start;
            buffer_start += required_size;
            return result;
        }








        return new char[required_size];
    }

}; // namespace chaos
