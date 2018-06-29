#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{

	class GPURenderbuffer : public GPUResource
	{
	public:

		/** constructor */
		GPURenderbuffer(GLuint in_id = 0);
		/** destructor */
		virtual ~GPURenderbuffer();

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsRenderbuffer(renderbuffer_id) == GL_TRUE; }
		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return renderbuffer_id; }

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint renderbuffer_id = 0;
	};


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


	class GPUFramebufferGenerator
	{
	public:

		/** generate the framebuffer */
		GPUFramebuffer * GenerateFramebuffer();

		/** Clear the generator */
		void Clear();





	};




#if 0
	GLuint framebuffer = 0;
	glGenFramebuffers(1, &framebuffer);

	GLuint depth_renderbuffer = 0;
	glGenRenderbuffers(1, &depth_renderbuffer);

	glNamedRenderbufferStorage(depth_renderbuffer, GL_DEPTH24_STENCIL8, 512, 512);

	GLuint color_renderbuffer = 0;
	glGenRenderbuffers(1, &color_renderbuffer);
	glNamedRenderbufferStorage(color_renderbuffer, GL_RGBA, 512, 512);

	glNamedFramebufferRenderbuffer(framebuffer, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, color_renderbuffer);
	glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, texture_id, texture_level);
	glCheckNamedFramebufferStatus(framebuffer, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;

#endif


}; // namespace chaos
