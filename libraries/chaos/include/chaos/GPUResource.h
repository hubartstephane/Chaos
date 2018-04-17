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

		/** called whenever the object is being released (returns true whether the callback must be removed from the list) */
		virtual bool OnResourceReleased(GPUResource * object, bool destruction);
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
		virtual bool OnResourceReleased(GPUResource * object, bool destruction) override;

	public:

		/** the callbacks function */
		std::function<bool(GPUResource *, bool)> released_func;
	};

	// ==============================================================
	// GPUResource
	// ==============================================================

	class GPUResource : public ReferencedObject
	{
	public:

		/** destructor */
		virtual ~GPUResource();
		/** cleaning the resource (public function) */
		void Release();

		/** add a callbacks (no verification for duplicate) */
		void AddReleaseCallback(GPUResourceCallbacks * callback);
		/** remove a callback (only the very firsr instance) */
		void RemoveReleaseCallback(GPUResourceCallbacks * callback);

	protected:

		/** cleaning the resource (method to derive) */
		virtual bool DoRelease();
		/** called whenever the resource is being destroyed / released. Call pending callbacks */
		void TriggerReleaseCallbacks(bool destruction);

	protected:

		/** the callbacks that are to be called when resource is being released */
		std::vector<boost::intrusive_ptr<GPUResourceCallbacks>> callbacks;
	};

}; // namespace chaos
