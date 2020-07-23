#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUClasses.h>

#include <chaos/GPUFileResource.h>
#include <chaos/GPUProgramData.h>

namespace chaos
{

	class GPUProgram : public GPUFileResource
	{
		friend class GPUResourceManager;

	public:

		/** constructor */
		GPUProgram(GLuint in_id = 0);
		/** destructor */
		virtual ~GPUProgram();

		/** use the program */
		bool UseProgram(class GPUProgramProviderBase const * uniform_provider) const;

		/** returns the GL name of the resource */
		GLuint GetResourceID() const { return program_id; }
		/** returns true whether the resource is valid */
		bool IsValid() const { return glIsProgram(program_id) == GL_TRUE;}
		/** get the program data */
		GPUProgramData const & GetProgramData() const { return program_data; }

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the resource id */
		GLuint program_id = 0;
		/** the program data */
		GPUProgramData program_data;
	};

}; // namespace chaos

