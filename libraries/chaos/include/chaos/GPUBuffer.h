#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{
	class GPUBuffer : public GPUResource
	{
	public:

		/** constructor (create its own resource) */
		GPUBuffer();
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUBuffer(GLuint in_id, bool in_ownership = true);
		/** destructor */
		virtual ~GPUBuffer();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return buffer_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

		/** create an OpenGL resource */
		bool CreateResource();
		/** Initialize from GL Resource */
		bool SetResource(GLuint in_id, bool in_ownership);

		/** update the data of the buffer */
		bool SetBufferData(char const * buffer, size_t size);
		/** get the size of the buffer */
		size_t GetBufferSize() const;

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint buffer_id = 0;
		/** the size of the buffer */
		size_t buffer_size = 0;
		/** whether the object has ownership of the GL resource */
		bool ownership = true;
	};

}; // namespace chaos