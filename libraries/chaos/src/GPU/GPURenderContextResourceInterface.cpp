#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPURenderContextResourceInterface::GPURenderContextResourceInterface(GPURenderContext* in_gpu_render_context) :
		gpu_render_context(in_gpu_render_context)
	{
		assert(in_gpu_render_context != nullptr);
	}

	GPUDevice* GPURenderContextResourceInterface::GetGPUDevice() const
	{
		if (gpu_render_context == nullptr)
			return nullptr;
		return gpu_render_context->GetGPUDevice();
	}

	bool GPURenderContextResourceInterface::IsRenderContextCurrent() const
	{
		if (gpu_render_context == nullptr)
			return false;
		return gpu_render_context->IsRenderContextCurrent();
	}

}; // namespace chaos
