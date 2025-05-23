#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPUProgramStringSourceGenerator::GPUProgramStringSourceGenerator(char const * src)
	{
		size_t len = strlen(src);
		if (len > 0)
		{
			buffer = SharedBufferPolicy<char>::NewBuffer(len + 1);
			if (buffer != nullptr)
				memcpy(buffer.data, src, len + 1);
		}
	}

	GPUProgramStringSourceGenerator::GPUProgramStringSourceGenerator(Buffer<char> in_buffer) : buffer(in_buffer)
	{

	}

	GPUProgramFileSourceGenerator::GPUProgramFileSourceGenerator(FilePathParam const & path)
	{
		buffer = FileTools::LoadFile(path, LoadFileFlag::ASCII | LoadFileFlag::NO_ERROR_TRACE);
		if (buffer == nullptr)
			GLLog::Error("GPUProgramFileSourceGenerator: fail to load [%s]", path.GetResolvedPath().string().c_str());
	}

}; // namespace chaos
