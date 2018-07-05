#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUFileResource.h>
#include <chaos/TextureDescription.h>

namespace chaos
{

	class GPUSurface : public GPUFileResource
	{
	public:

		/** get the description of the surface */
		virtual SurfaceDescription const & GetSurfaceDescription() const = 0;
	};

}; // namespace chaos
