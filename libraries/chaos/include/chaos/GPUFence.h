#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GPUResource.h>

namespace chaos
{

	class GPUFence : public GPUResource
	{
	public:



		/** constructor */
		GPUFence(GLsync in_fence = nullptr);
		/** destructor */
		virtual ~GPUFence();

		/** wait until the fence is signaled */
		bool WaitForCompletion(float timeout);
		/** returns true whether the resource is valid */
		bool IsValid() const { return (fence != nullptr) && glIsSync(fence); }

	protected:

		/** cleaning the object */
		virtual bool DoRelease() override;

	protected:

		/** the fence object */
		GLsync fence = nullptr;

	};

}; // namespace chaos
