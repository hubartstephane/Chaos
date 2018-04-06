#include <chaos/GPUResource.h>

namespace chaos
{

	void GPUResourceCallbacks::OnResourceReleased(GPUResource const * object, bool destruction)
	{
		assert(object != nullptr);
	}

	void GPUResourceAutoCallbacks::OnResourceReleased(GPUResource const * object, bool destruction)
	{
		assert(object != nullptr);
		if (released_func)
			released_func(object, destruction);
	}

	GPUResource::~GPUResource()
	{
		OnReleased(true);	
	}
	
	void GPUResource::Release()
	{	
		DoRelease();
		OnReleased(false);
	}

	
	void GPUResource::DoRelease()
	{
	
		
	}

	void GPUResource::OnReleased(bool destruction)
	{
		size_t count = callbacks.size();
		if (count == 0)
			return;

		for (size_t i = count ; i != 0 ; --i)
		{
			size_t index = count - 1;
			callbacks[index]->OnResourceReleased(this, destruction);		
		}	
	}

}; // namespace chaos
