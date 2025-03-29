#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	bool GPUDevice::Initialize(JSONReadConfiguration config)
	{
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::INITIALIZATION, false)) // do not recursively update all child objects
			return false;
		return OnInitialize(config);
	}

	void GPUDevice::Finalize()
	{

	}

	bool GPUDevice::OnInitialize(JSONReadConfiguration config)
	{
		return true;
	}

}; // namespace chaos
