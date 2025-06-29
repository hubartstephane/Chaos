#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	GPUDevice::GPUDevice():
		buffer_pool(this),
		texture_pool(this)
	{
	}

	GPUDevice::~GPUDevice()
	{
		assert(render_contexts.size() == 0);
	}

	bool GPUDevice::Initialize(JSONReadConfiguration config)
	{
		if (!ReadConfigurableProperties(ReadConfigurablePropertiesContext::Initialization, false)) // do not recursively update all child objects
			return false;
		return OnInitialize(config);
	}

	void GPUDevice::Finalize()
	{
		buffer_pool.ClearPool();
		texture_pool.ClearPool();
	}

	bool GPUDevice::OnInitialize(JSONReadConfiguration config)
	{
		return true;
	}

	GPUBuffer * GPUDevice::CreateBuffer(size_t in_buffer_size, GPUBufferFlags in_flags)
	{
		return buffer_pool.CreateBuffer(in_buffer_size, in_flags);
	}

	void GPUDevice::OnBufferUnused(GPUBuffer * in_buffer)
	{
		buffer_pool.OnBufferUnused(in_buffer);
	}

	GPUTexture* GPUDevice::CreateTexture(TextureDescription const& in_texture_description)
	{
		return texture_pool.CreateTexture(in_texture_description);
	}

	void GPUDevice::OnTextureUnused(GPUTexture * in_texture)
	{
		texture_pool.OnTextureUnused(in_texture);
	}

	bool GPUDevice::DoTick(float delta_time)
	{
		buffer_pool.Tick(delta_time);
		texture_pool.Tick(delta_time);
		return true;
	}

	GPURenderContext * GPUDevice::CreateRenderContext(Window* in_window)
	{
		GPURenderContext * result = new GPURenderContext(this, in_window);
		if (result == nullptr)
			return nullptr;
		render_contexts.push_back(result);
		return result;
	}

	void GPUDevice::OnTextureDestroyed(GLuint in_texture_id)
	{
	}

	void GPUDevice::OnBufferDestroyed(GLuint in_buffer_id)
	{
		for (auto & render_context : render_contexts)
			render_context->OnBufferDestroyed(in_buffer_id);
	}

	void GPUDevice::OnProgramDestroyed(GLuint in_program_id)
	{
		for (auto & render_context : render_contexts)
			render_context->OnProgramDestroyed(in_program_id);
	}
	
	void GPUDevice::OnRenderContextDestroyed(GPURenderContext * in_render_context)
	{
		auto it = std::find_if(render_contexts.begin(), render_contexts.end(), [in_render_context](shared_ptr<GPURenderContext> & in_context)
		{
			return (in_context == in_render_context);
		});
		if (it != render_contexts.end())
			render_contexts.erase(it);
	}

}; // namespace chaos
