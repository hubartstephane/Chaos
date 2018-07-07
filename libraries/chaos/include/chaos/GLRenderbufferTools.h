#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPURenderbuffer.h>

namespace chaos
{

	/**
	* GLRenderbufferTools : used to have some generic functions for OpenGL
	**/

	class GLRenderbufferTools
	{
	public:

		/** create a renderbuffer */
		static GPURenderbuffer * GenRenderbufferObject();
	};

}; // namespace chaos
