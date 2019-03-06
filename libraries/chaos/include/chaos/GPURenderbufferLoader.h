#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUFileResource.h>
#include <chaos/GPURenderbuffer.h>

namespace chaos
{

	/**
	* GPURenderbufferLoader : used to have some generic functions for OpenGL
	**/

	class GPURenderbufferLoader : protected FileResourceFriend  // give the hability to change path and names to the resource
	{
	public:

		/** create a renderbuffer */
		GPURenderbuffer * GenRenderbufferObject(PixelFormat const & pixel_format, glm::ivec2 const & size);
	};

}; // namespace chaos
