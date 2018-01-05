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

		/** cleaning the object */
		virtual void Release() override;

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return resource_id; }

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsBuffer(resource_id) == GL_TRUE;}

	protected:

		/** the resource id */
		GLuint resource_id;
	};

}; // namespace chaos
