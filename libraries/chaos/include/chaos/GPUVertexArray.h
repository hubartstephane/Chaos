#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{

	class GPUVertexArray : public GPUResource
	{
	public:

		/** constructor (create its own resource) */
		GPUVertexArray();
		/** constructor (reference a given resource). Call this function with 0 if you do not want to create resource at all */
		GPUVertexArray(GLuint in_id, bool in_ownership);

		/** destructor */
		virtual ~GPUVertexArray();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return vertex_array_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const;

		/** create an OpenGL resource */
		bool CreateResource();
		/** Initialize from GL Resource */
		bool SetResource(GLuint in_id, bool in_ownership);

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint vertex_array_id = 0;
		/** whether the object has ownership of the GL resource */
		bool ownership = true;
	};


}; // namespace chaos
