#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{

	class VertexArray : public GPUResource
	{
	public:

		/** constructor */
		VertexArray(GLuint in_id = 0);
		/** destructor */
		virtual ~VertexArray();

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return vertex_array_id; }

		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsVertexArray(vertex_array_id) == GL_TRUE;}

	protected:

		/** cleaning the object */
		virtual void DoRelease() override;

	protected:

		/** the resource id */
		GLuint vertex_array_id = 0;
	};


}; // namespace chaos
