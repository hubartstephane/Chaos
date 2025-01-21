#include "chaos/Chaos.h"

class GPUDevice;
class GPUResource;
class GPUBuffer;

class GPUFileResource;
class GPUProgram;
class GPURenderMaterial;
class GPUFramebuffer;
class GPUSurface;
class GPURenderbuffer;
class GPUTexture;

class GPURenderContext;
class GPUQuery;
class GPUFence;
class GPUVertexArray;





//---------------------------------------------------------


class GPUResource : public chaos::Object
{


};

//---------------------------------------------------------

// shared among contexts
class GPUBuffer : public GPUResource
{
	friend class GPUDevice;

public:

	virtual void SubReference() override;

	size_t GetMemoryUsage() const;

protected:
public:

	GPUBuffer(GPUDevice* in_device, size_t in_size) :
		device(in_device)
	{}

protected:

	GPUDevice * device = nullptr;

};

//---------------------------------------------------------

class GPUFileResource : public GPUResource {};


//---------------------------------------------------------

class GPUSurface : public GPUFileResource
{

protected:

	GPUSurface(GPUDevice* in_device) :
		device(in_device)
	{}

protected:

	GPUDevice * device = nullptr;

};

//---------------------------------------------------------

class GPURenderbuffer : public GPUSurface
{
	friend class GPUDevice;

public:

	virtual void SubReference() override;

	size_t GetMemoryUsage() const;

protected:
public:

	GPURenderbuffer(GPUDevice* in_device) :
		GPUSurface(in_device)
	{}
};


//---------------------------------------------------------

class GPUTexture : public GPUSurface
{
	friend class GPUDevice;

public:

	virtual void SubReference() override;

	size_t GetMemoryUsage() const;

protected:
public:

	GPUTexture(GPUDevice* in_device) :
		GPUSurface(in_device)
	{}
};


//---------------------------------------------------------

class GPUProgram : public GPUFileResource
{
	friend class GPUDevice;

protected:

	GPUProgram(GPUDevice* in_device) :
		device(in_device)
	{}

protected:

	GPUDevice* device = nullptr;
};

//---------------------------------------------------------

class GPURenderMaterial : public GPUFileResource
{
	friend class GPUDevice;

protected:

	GPURenderMaterial(GPUDevice * in_device):
		device(in_device)
	{}

protected:

	GPUDevice * device = nullptr;

};

//---------------------------------------------------------

class GPUFence : public GPUResource
{
	friend class GPURenderContext;

protected:

	GPUFence(GPURenderContext* in_render_context):
		render_context(in_render_context)
	{}

protected:

	GPURenderContext* render_context = nullptr;

};

//---------------------------------------------------------

class GPUQuery : public GPUResource
{
	friend class GPURenderContext;

protected:

	GPUQuery(GPURenderContext* in_render_context) :
		render_context(in_render_context)
	{}

protected:

	GPURenderContext* render_context = nullptr;
};

//---------------------------------------------------------

class GPUVertexArray : public GPUResource
{
	friend class GPURenderContext;

protected:

	GPUVertexArray(GPURenderContext* in_render_context) :
		render_context(in_render_context)
	{}

protected:

	GPURenderContext* render_context = nullptr;
};

//---------------------------------------------------------

class GPUFramebuffer : public GPUResource
{
	friend class GPURenderContext;

protected:

	GPUFramebuffer(GPURenderContext* in_render_context) :
		render_context(in_render_context)
	{}

protected:

	GPURenderContext* render_context = nullptr;
};



template<typename T>
class GPUDeviceUnusedResourceInfo
{
public:

	T * resource = nullptr;

	double unused_start_time = 0.0;
};

template<typename T>
class GPUDeviceResourceInfo
{
public:

	size_t GetUsedResourcesCount() const
	{
		return resources.size() - unused_resources.size();
	}

	size_t GetUnusedResourcesCount() const
	{
		return unused_resources.size();
	}

	size_t GetUsedResourcesMemoryUsage() const
	{
		return resources_memory_usage - unused_resources_memory_usage;
	}

	size_t GetUnusedResourcesMemoryUsage() const
	{
		return unused_resources_memory_usage;
	}

	void PurgeUnusedResources(double current_time, float max_unused_time)
	{
		auto it = std::remove_if(unused_resources.begin(), unused_resources.end(), [this, current_time, max_unused_time](GPUDeviceUnusedResourceInfo<T> const& info)
		{
			if (current_time - info.unused_start_time > max_unused_time)
			{
				unused_resources_memory_usage -= info.resource->GetMemoryUsage();
				RemoveResource(info.resource);
				return true;
			}
			return false;
		});
		unused_resources.erase(it, unused_resources.end());
	}

	void RemoveResource(T * in_resource)
	{
		auto it = std::find_if(resources.begin(), resources.end(), [in_resource](chaos::shared_ptr<T> const& resource)
		{
			return resource == in_resource;
		});

		if (it != resources.end())
		{
			resources_memory_usage -= in_resource->GetMemoryUsage();
			resources.erase(it);
		}
	}

	template<typename... PARAMS>
	T * CreateResource(GPUDevice * in_device, PARAMS && ...params)
	{
		T * result = new T(in_device, std::forward<PARAMS>(params)...);
		if (result != nullptr)
		{
			resources.push_back(result);
			resources_memory_usage += result->GetMemoryUsage();
		}
		return result;
	}

	void OnResourceUnused(T * in_resource)
	{
	}













public:

	std::vector<chaos::shared_ptr<T>> resources;

	std::vector<GPUDeviceUnusedResourceInfo<T>> unused_resources;

	size_t resources_memory_usage = 0;

	size_t unused_resources_memory_usage = 0;
};

class GPUDevice : public chaos::Tickable // a single for the whole system
{
	friend class GPUBuffer;
	friend class GPUTexture;
	friend class GPURenderbuffer;

public:

	GPUDevice()
	{
#ifdef _DEBUG
		assert(++instance_count == 1);
#endif
	}

	// GPURenderer * CreateRenderContext();

	GPUBuffer * CreateBuffer(size_t bufsize);

	GPUTexture* CreateTexture();

	GPURenderbuffer* CreateRenderbuffer();

	virtual bool DoTick(float delta_time) override;



	size_t GetUsedBuffersCount() const
	{
		return buffers.GetUsedResourcesCount();
	}

	size_t GetUnusedBuffersCount() const
	{
		return buffers.GetUnusedResourcesCount();
	}

	size_t GetUsedBuffersMemoryUsage() const
	{
		return buffers.GetUsedResourcesMemoryUsage();
	}

	size_t GetUnusedBuffersMemoryUsage() const
	{
		return buffers.GetUnusedResourcesMemoryUsage();
	}

	size_t GetUsedTexturesCount() const
	{
		return textures.GetUsedResourcesCount();
	}

	size_t GetUnusedTexturesCount() const
	{
		return textures.GetUnusedResourcesCount();
	}

	size_t GetUsedTexturesMemoryUsage() const
	{
		return textures.GetUsedResourcesMemoryUsage();
	}

	size_t GetUnusedTexturesMemoryUsage() const
	{
		return textures.GetUnusedResourcesMemoryUsage();
	}

	size_t GetUsedRenderbuffersCount() const
	{
		return render_buffers.GetUsedResourcesCount();
	}

	size_t GetUnusedRenderbuffersCount() const
	{
		return render_buffers.GetUnusedResourcesCount();
	}

	size_t GetUsedRenderbuffersMemoryUsage() const
	{
		return render_buffers.GetUsedResourcesMemoryUsage();
	}

	size_t GetUnusedRenderbuffersMemoryUsage() const
	{
		return render_buffers.GetUnusedResourcesMemoryUsage();
	}

protected:

	void PurgeUnusedResources(float delta_time);

	void DoPurgeUnusedResources(float max_unused_time);

	void OnBufferUnused(GPUBuffer * in_buffer);

	void OnTextureUnused(GPUTexture* in_texture);

	void OnRenderbufferUnused(GPURenderbuffer* in_renderbuffer);





protected:

	GPUDeviceResourceInfo<GPUBuffer> buffers;

	GPUDeviceResourceInfo<GPUTexture> textures;

	GPUDeviceResourceInfo<GPURenderbuffer> render_buffers;

	double current_time = 0.0;

	float purge_max_timer = 10.0f;

	float current_purge_time = 0.0f;

	uint64_t rendering_timestamp = 0; // resources maybe shared by several GPURenderer


#ifdef _DEBUG
	int instance_count = 0;
#endif
};

//---------------------------------------------------------

class GPUDrawStats
{
public:

	int draw_calls = 0;
	int vertices   = 0;
};

//---------------------------------------------------------

class GPURenderContext
{
public:

	GPUFence * CreateFence();

	GPUQuery* CreateQuery();

	GPUVertexArray* CreateVertexArray();

	GPUFramebuffer* CreateFramebuffer();

protected:

	GLFWwindow * context = nullptr;

	GPUDrawStats current_frame_stats;

	int fence_count = 0;

	int query_count = 0;

	int vertex_array_count = 0;

	int framebuffer_count = 0;
};


// -----------------------------------------------------------------------

#if 0

GPURenderContext* GPUDevice::CreateRenderContext()
{
	return new GPURenderContext;
}

#endif

bool GPUDevice::DoTick(float delta_time)
{
	current_time += (double)delta_time;

	PurgeUnusedResources(delta_time);
	return true;
}

void GPUDevice::PurgeUnusedResources(float delta_time)
{
	if (purge_max_timer > 0.0f)
	{
		current_purge_time -= delta_time;
		if (current_purge_time < 0.0f)
		{
			DoPurgeUnusedResources(purge_max_timer);
			current_purge_time = purge_max_timer;
		}
	}
}

void GPUDevice::DoPurgeUnusedResources(float max_unused_time)
{
	buffers.PurgeUnusedResources(current_time, max_unused_time);
	textures.PurgeUnusedResources(current_time, max_unused_time);
	render_buffers.PurgeUnusedResources(current_time, max_unused_time);
}

GPUBuffer* GPUDevice::CreateBuffer(size_t bufsize)
{
	return buffers.CreateResource(this, bufsize);
}

GPUTexture* GPUDevice::CreateTexture()
{
	return textures.CreateResource(this);
}

GPURenderbuffer* GPUDevice::CreateRenderbuffer()
{
	return render_buffers.CreateResource(this);
}

void GPUDevice::OnBufferUnused(GPUBuffer* in_buffer)
{
	buffers.OnResourceUnused(in_buffer);
}

void GPUDevice::OnTextureUnused(GPUTexture* in_texture)
{
	textures.OnResourceUnused(in_texture);
}

void GPUDevice::OnRenderbufferUnused(GPURenderbuffer* in_render_buffer)
{
	render_buffers.OnResourceUnused(in_render_buffer);
}





// -----------------------------------------------------------------------

void GPUBuffer::SubReference()
{
	--shared_count;
	if (shared_count == 0)
		OnLastReferenceLost();
	else if (shared_count == 1) // last reference is owned by GPUDevice
		device->OnBufferUnused(this);
}

size_t GPUBuffer::GetMemoryUsage() const
{
	return 0;
}

void GPUTexture::SubReference()
{
	--shared_count;
	if (shared_count == 0)
		OnLastReferenceLost();
	else if (shared_count == 1) // last reference is owned by GPUDevice
		device->OnTextureUnused(this);
}

size_t GPUTexture::GetMemoryUsage() const
{
	return 0;
}

void GPURenderbuffer::SubReference()
{
	--shared_count;
	if (shared_count == 0)
		OnLastReferenceLost();
	else if (shared_count == 1) // last reference is owned by GPUDevice
		device->OnRenderbufferUnused(this);
}

size_t GPURenderbuffer::GetMemoryUsage() const
{
	return 0;
}

// -----------------------------------------------------------------------

GPUFence* GPURenderContext::CreateFence()
{
	GPUFence* result = new GPUFence(this);
	if (result != nullptr)
		++fence_count;
	return result;
}

GPUQuery* GPURenderContext::CreateQuery()
{
	GPUQuery* result = new GPUQuery(this);
	if (result != nullptr)
		++query_count;
	return result;
}

GPUVertexArray* GPURenderContext::CreateVertexArray()
{
	GPUVertexArray* result = new GPUVertexArray(this);
	if (result != nullptr)
		++vertex_array_count;
	return result;
}

GPUFramebuffer* GPURenderContext::CreateFramebuffer()
{
	GPUFramebuffer* result = new GPUFramebuffer(this);
	if (result != nullptr)
		++framebuffer_count;
	return result;
}






















class DrawPass
{
public:

#if 0

	GPUDrawPass CreateChildDrawPass()
	{

	}
#endif


protected:

};




class WindowOpenGLTest : public chaos::Window
{
	CHAOS_DECLARE_OBJECT_CLASS(WindowOpenGLTest, Window);

protected:

	virtual bool OnDraw(chaos::GPURenderer * renderer, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{


		return true;
	}

};

int main(int argc, char ** argv, char ** env)
{
    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
