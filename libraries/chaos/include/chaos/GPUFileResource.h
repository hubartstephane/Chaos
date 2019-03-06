#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>
#include <chaos/FileResource.h>

namespace chaos
{
	class GPUFileResource : public FileResource<GPUResource>
	{
	};

}; // namespace chaos
