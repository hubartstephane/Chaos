#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{

	class GPUFence : public GPUResource
	{
	public:

#if 0

		/** constructor */
		GPUFence(GLuint in_id = 0, SurfaceDescription const & in_surface_description = SurfaceDescription());
		/** destructor */
		virtual ~GPUFence();

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsRenderbuffer(renderbuffer_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return renderbuffer_id; }
		/** get the description of the surface */
		SurfaceDescription const & GetSurfaceDescription() const override { return surface_description; }

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint renderbuffer_id = 0;

#endif

	};

}; // namespace chaos
