#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUTexture.h>
#include <chaos/GPUResource.h>
#include <chaos/GPURenderbuffer.h>

namespace chaos
{
	class GPUFramebuffer : public GPUResource
	{
	public:

		/** constructor */
		GPUFramebuffer(GLuint in_id = 0);
		/** destructor */
		virtual ~GPUFramebuffer();

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsFramebuffer(framebuffer_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return framebuffer_id; }

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint framebuffer_id = 0;
		/** private and shared resources */
		std::vector<boost::intrusive_ptr<GPUResource>> resources;
	};

}; // namespace chaos
