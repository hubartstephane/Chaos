#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUSurface.h>

namespace chaos
{

	class GPURenderbuffer : public GPUSurface
	{
	public:

		/** constructor */
		GPURenderbuffer(GLuint in_id = 0, SurfaceDescription const & in_surface_description = SurfaceDescription());
		/** destructor */
		virtual ~GPURenderbuffer();

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
		/** the description of the framebuffer */
		SurfaceDescription surface_description;
	};

}; // namespace chaos
