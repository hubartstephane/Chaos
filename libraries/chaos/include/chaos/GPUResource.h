#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>

namespace chaos
{

	// ==============================================================
	// CALLBACKS
	// ==============================================================

	class GPUResourceCallbacks : public ReferencedObject
	{
		friend class GPUResource;

	protected:

		/** called whenever the object is being released */
		virtual void OnResourceReleased(GPUResource const * object, bool destruction){}
	};

	class GPUResourceAutoCallbacks : public GPUResourceCallbacks
	{

	public:

		/** default constructor */
		GPUResourceAutoCallbacks() = default;
		/** assignation constructor */
		template<typename U>
		GPUResourceAutoCallbacks(U & in_released_func) :
			released_func(in_released_func)
		{
		}

	protected:

		/** called whenever a sound is finished */
		virtual void OnResourceReleased(GPUResource const * object, bool destruction) override;

	public:

		/** the callbacks function */
		std::function<void(GPUResource const *, bool)> released_func;
	};

	// ==============================================================
	// GPUResource
	// ==============================================================

	class GPUResource : public ReferencedObject
	{
	public:

		/** destructor */
		virtual ~GPUResource();
		/** cleaning the resource */
		void Release();

	protected:

		/** cleaning the resource (method to derive) */
		virtual void DoRelease();
		/** called whenever the resource is being destroyed / released */
		void OnReleased(bool destruction);

	protected:

		/** the callbacks that are to be called when resource is being released */
		std::vector<boost::intrusive_ptr<GPUResourceCallbacks>> callbacks;


	};

}; // namespace chaos
