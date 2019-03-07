#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>
#include <chaos/FileResource.h>

namespace chaos
{
	class GPUFileResource : public GPUResource, public NamedResource, public FileResource
	{
	};

}; // namespace chaos
