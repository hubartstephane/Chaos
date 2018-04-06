#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{
	class GPUBuffer : public GPUResource
	{
	public:

		/** constructor */
		GPUBuffer(GLuint in_id = 0);
		/** destructor */
		virtual ~GPUBuffer();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return resource_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsBuffer(resource_id) == GL_TRUE;}

		/** update the data of the buffer */
		bool SetBufferData(char const * buffer, size_t size);
		/** get the size of the buffer */
		GLint GetBufferSize() const;

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint resource_id = 0;
	};

}; // namespace chaos