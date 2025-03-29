#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	GPUDeviceResourceInterface::GPUDeviceResourceInterface(GPUDevice* in_gpu_device) :
		gpu_device(in_gpu_device)
	{
		assert(gpu_device != nullptr);
	}

}; // namespace chaos
