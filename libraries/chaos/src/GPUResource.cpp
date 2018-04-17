#include <chaos/GPUResource.h>

namespace chaos
{
	bool GPUResourceCallbacks::OnResourceReleased(GPUResource * object, bool destruction)
	{
		assert(object != nullptr);
		return true;
	}

	bool GPUResourceAutoCallbacks::OnResourceReleased(GPUResource * object, bool destruction)
	{
		assert(object != nullptr);
		if (released_func)
			return released_func(object, destruction);
		return true;
	}

	GPUResource::~GPUResource()
	{
		TriggerReleaseCallbacks(true);
	}

	void GPUResource::Release()
	{
		if (DoRelease())
			TriggerReleaseCallbacks(false);
	}

	bool GPUResource::DoRelease()
	{
		return false;
	}

	void GPUResource::TriggerReleaseCallbacks(bool destruction)
	{
		for (size_t i = callbacks.size(); i != 0; --i)
		{
			size_t index = i - 1;
			if (callbacks[index]->OnResourceReleased(this, destruction))
			{
				if (index < callbacks.size() - 1)
					callbacks[index] = callbacks[callbacks.size() - 1];
				callbacks.pop_back();
			}
		}
	}

	void GPUResource::AddReleaseCallback(GPUResourceCallbacks * callback)
	{
		assert(callback != nullptr);
		callbacks.push_back(callback);
	}

	void GPUResource::RemoveReleaseCallback(GPUResourceCallbacks * callback)
	{
		assert(callback != nullptr);

		size_t count = callbacks.size();
		for (size_t i = 0; i < count; ++i)
		{
			if (callbacks[i] == callback)
			{
				if (i < count - 1)
					callbacks[i] = callbacks[callbacks.size() - 1];
				callbacks.pop_back();
				return;
			}
		}
	}

}; // namespace chaos
