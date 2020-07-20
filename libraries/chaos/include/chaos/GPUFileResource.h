#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>
#include <chaos/GPUResource.h>
#include <chaos/FileResource.h>
#include <chaos/NamedObject.h>

namespace chaos
{
	class GPUFileResource : public GPUResource, public NamedObject, public FileResource
	{
	};

}; // namespace chaos

