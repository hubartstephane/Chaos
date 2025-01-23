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

enum class GPUBufferFlags
{
	Static  = (1 << 0), // default value
	Dynamic = (1 << 1)
};



CHAOS_DECLARE_ENUM_BITMASK_METHOD(GPUBufferFlags);

static chaos::EnumTools::EnumBitmaskMetaData<GPUBufferFlags> const GPUBufferFlags_bitmask_metadata =
{
	{ GPUBufferFlags::Static, GPUBufferFlags::Dynamic}
};

CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(GPUBufferFlags, &GPUBufferFlags_bitmask_metadata);

// shared among contexts
class GPUBuffer : public GPUResource
{
	template<typename T, typename UNUSED_RESOURCE_FINDER>
	friend class GPUDeviceResourceManager;

public:

	virtual void SubReference() override;

	size_t GetMemoryUsage() const;

	bool IsStatic() const
	{
		return !HasAnyFlags(flags, GPUBufferFlags::Dynamic);
	}

	bool IsDynamic() const
	{
		return HasAnyFlags(flags, GPUBufferFlags::Dynamic);
	}

protected:

	GPUBuffer(GPUDevice* in_device, size_t in_size, GPUBufferFlags in_flags = GPUBufferFlags::Static):
		device(in_device),
		size(in_size),
		flags(in_flags)
	{}



protected:

	GPUDevice* device = nullptr;

	size_t size = 0;

	GPUBufferFlags flags = GPUBufferFlags::Static;
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
	template<typename T, typename UNUSED_RESOURCE_FINDER>
	friend class GPUDeviceResourceManager;

public:

	virtual void SubReference() override;

	size_t GetMemoryUsage() const;

protected:

	GPURenderbuffer(GPUDevice* in_device) :
		GPUSurface(in_device)
	{}
};


//---------------------------------------------------------

class GPUTexture : public GPUSurface
{
	template<typename T, typename UNUSED_RESOURCE_FINDER>
	friend class GPUDeviceResourceManager;

public:

	virtual void SubReference() override;

	size_t GetMemoryUsage() const;

protected:

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

//---------------------------------------------------------

template<typename T>
class GPUDeviceUnusedResourceInfo
{
public:

	T * resource = nullptr;

	double unused_start_time = 0.0;
};

//---------------------------------------------------------

enum class GPUDeviceResourceType
{
	Used,
	Unused,
	Total
};

template<typename T, typename UNUSED_RESOURCE_FINDER>
class GPUDeviceResourceManager
{
public:

	~GPUDeviceResourceManager()
	{
		for (chaos::shared_ptr<T> & resource : resources) // detach the resources from the device
			resource->device = nullptr;
	}

	size_t GetResourcesCount(GPUDeviceResourceType type) const
	{
		switch(type)
		{
		case GPUDeviceResourceType::Used:
			return resources.size() - unused_resources.size();
		case GPUDeviceResourceType::Unused:
			return unused_resources.size();
		case GPUDeviceResourceType::Total:
			return resources.size();
		}
		assert(0);
		return 0;
	}

	size_t GetMemoryUsage(GPUDeviceResourceType type) const
	{
		switch (type)
		{
		case GPUDeviceResourceType::Used:
			return memory_usage - unused_memory_usage;
		case GPUDeviceResourceType::Unused:
			return unused_memory_usage;
		case GPUDeviceResourceType::Total:
			return memory_usage;
		}
		assert(0);
		return 0;
	}

	void PurgeUnusedResources(double current_time, float purge_max_age)
	{
		if (current_time - min_unused_start_time < (double)purge_max_age)
			return;

		min_unused_start_time = std::numeric_limits<double>::max();

		auto it = std::remove_if(unused_resources.begin(), unused_resources.end(), [this, current_time, purge_max_age](GPUDeviceUnusedResourceInfo<T> const& info)
		{
			if (current_time - info.unused_start_time >= purge_max_age)
			{
				unused_memory_usage -= info.resource->GetMemoryUsage();
				RemoveResource(info.resource);
				return true;
			}
			min_unused_start_time = std::min(min_unused_start_time, info.unused_start_time);
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
			memory_usage -= in_resource->GetMemoryUsage();
			resources.erase(it);
		}
	}

	template<typename... PARAMS>
	T * CreateResource(GPUDevice * in_device, PARAMS && ...params)
	{
		// try to get an unused resource
		auto it = unused_resource_finder.FindMatchingUnusedResourceIterator(unused_resources, std::forward<PARAMS>(params)...);
		if (it != unused_resources.end())
		{
			T* result = it->resource;
			unused_memory_usage -= result->GetMemoryUsage();
			unused_resources.erase(it);
			return result;
		}
		// create a new resource
		T * result = new T(in_device, std::forward<PARAMS>(params)...);
		if (result != nullptr)
		{
			resources.push_back(result);
			memory_usage += result->GetMemoryUsage();
		}
		return result;
	}

	void OnResourceUnused(double current_time, T * in_resource)
	{
		size_t resource_memory_usage = in_resource->GetMemoryUsage();

		if (max_unused_memory_usage > 0 && unused_memory_usage + resource_memory_usage > max_unused_memory_usage)
		{
			RemoveResource(in_resource);
		}
		else
		{
			unused_resources.push_back({ in_resource, current_time });
			unused_memory_usage += resource_memory_usage;
			min_unused_start_time = std::min(min_unused_start_time, current_time);
		}
	}

public:

	UNUSED_RESOURCE_FINDER unused_resource_finder;

	std::vector<chaos::shared_ptr<T>> resources;

	std::vector<GPUDeviceUnusedResourceInfo<T>> unused_resources;

	size_t memory_usage = 0;

	size_t unused_memory_usage = 0;

	size_t max_unused_memory_usage = 0;

	double min_unused_start_time = std::numeric_limits<double>::max();
};

// --------------------------------------------------------------------------

class GPUDeviceUnusedBufferFinder
{
public: 

	template<typename... PARAMS>
	auto FindMatchingUnusedResourceIterator(std::vector<GPUDeviceUnusedResourceInfo<GPUBuffer>>& unused_resources, size_t in_size, GPUBufferFlags in_flags) const
	{
		bool want_static_buffer = (!HasAnyFlags(in_flags, GPUBufferFlags::Dynamic));

		auto result = unused_resources.end();

		for (auto it = unused_resources.begin() ; it != unused_resources.end() ; ++it)
		{
			if (it->resource->IsStatic() != want_static_buffer)
				continue;
			if (it->resource->GetMemoryUsage() < in_size)
				continue;

			return it;
		}


		return result;
	}

};

using GPUDeviceBufferManager = GPUDeviceResourceManager<GPUBuffer, GPUDeviceUnusedBufferFinder>;

// --------------------------------------------------------------------------

class GPUDeviceUnusedTextureFinder
{
public:

	template<typename... PARAMS>
	auto FindMatchingUnusedResourceIterator(std::vector<GPUDeviceUnusedResourceInfo<GPUTexture>>& unused_resources, PARAMS && ...params) const
	{
		return unused_resources.end();
	}

};

using GPUDeviceTextureManager = GPUDeviceResourceManager<GPUTexture, GPUDeviceUnusedTextureFinder>;


// --------------------------------------------------------------------------

class GPUDeviceUnusedRenderbufferFinder
{
public:

	template<typename... PARAMS>
	auto FindMatchingUnusedResourceIterator(std::vector<GPUDeviceUnusedResourceInfo<GPURenderbuffer>>& unused_resources, PARAMS && ...params) const
	{
		return unused_resources.end();
	}

};

using GPUDeviceRenderbufferManager = GPUDeviceResourceManager<GPURenderbuffer, GPUDeviceUnusedRenderbufferFinder>;

// --------------------------------------------------------------------------





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

	GPURenderContext * CreateRenderContext(chaos::Window* in_window);

	GPUBuffer * CreateBuffer(size_t in_size, GPUBufferFlags in_flags = GPUBufferFlags::Static);

	GPUTexture* CreateTexture();

	GPURenderbuffer* CreateRenderbuffer();

	virtual bool DoTick(float delta_time) override;


	bool IsAnyContextActive() const;




	size_t GetBufferCount(GPUDeviceResourceType type) const
	{
		return buffers.GetResourcesCount(type);
	}

	size_t GetBufferMemoryUsage(GPUDeviceResourceType type) const
	{
		return buffers.GetMemoryUsage(type);
	}

	size_t GetTextureCount(GPUDeviceResourceType type) const
	{
		return textures.GetResourcesCount(type);
	}

	size_t GetTextureMemoryUsage(GPUDeviceResourceType type) const
	{
		return textures.GetMemoryUsage(type);
	}


	size_t GetRenderbufferCount(GPUDeviceResourceType type) const
	{
		return render_buffers.GetResourcesCount(type);
	}

	size_t GetRenderbufferMemoryUsage(GPUDeviceResourceType type) const
	{
		return render_buffers.GetMemoryUsage(type);
	}







protected:

	void PurgeUnusedResources();

	void OnBufferUnused(GPUBuffer * in_buffer);

	void OnTextureUnused(GPUTexture* in_texture);

	void OnRenderbufferUnused(GPURenderbuffer* in_renderbuffer);





protected:

	GPUDeviceBufferManager buffers;

	GPUDeviceTextureManager textures;

	GPUDeviceRenderbufferManager render_buffers;

	double current_time = 0.0;

	float purge_max_age = 10.0f;

	uint64_t rendering_timestamp = 0; // resources maybe shared by several GPURenderContext


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
	friend class GPUDevice;

public:

	bool IsContextActive() const;

	GPUFence * CreateFence();

	GPUQuery* CreateQuery();

	GPUVertexArray* CreateVertexArray();

	GPUFramebuffer* CreateFramebuffer();

protected:

	GPURenderContext(GPUDevice * in_device, chaos::Window * in_window);

protected:

	GPUDevice* device = nullptr;

	GLFWwindow * context = nullptr;

	GPUDrawStats current_frame_stats;

	int fence_count = 0;

	int query_count = 0;

	int vertex_array_count = 0;

	int framebuffer_count = 0;
};


// -----------------------------------------------------------------------

GPURenderContext* GPUDevice::CreateRenderContext(chaos::Window * in_window)
{
	assert(IsAnyContextActive());
	return new GPURenderContext(this, in_window);
}

bool GPUDevice::DoTick(float delta_time)
{
	assert(IsAnyContextActive());
	current_time += (double)delta_time;
	PurgeUnusedResources();
	return true;
}

void GPUDevice::PurgeUnusedResources()
{
	if (purge_max_age > 0.0f)
	{
		buffers.PurgeUnusedResources(current_time, purge_max_age);
		textures.PurgeUnusedResources(current_time, purge_max_age);
		render_buffers.PurgeUnusedResources(current_time, purge_max_age);
	}
}

bool GPUDevice::IsAnyContextActive() const
{
	return true;

//	return (glfwGetCurrentContext() != nullptr);
}

GPUBuffer* GPUDevice::CreateBuffer(size_t in_size, GPUBufferFlags in_flags)
{
	assert(IsAnyContextActive());
	return buffers.CreateResource(this, in_size, in_flags);
}

GPUTexture* GPUDevice::CreateTexture()
{
	assert(IsAnyContextActive());
	return textures.CreateResource(this);
}

GPURenderbuffer* GPUDevice::CreateRenderbuffer()
{
	assert(IsAnyContextActive());
	return render_buffers.CreateResource(this);
}

void GPUDevice::OnBufferUnused(GPUBuffer* in_buffer)
{
	buffers.OnResourceUnused(current_time, in_buffer);
}

void GPUDevice::OnTextureUnused(GPUTexture* in_texture)
{
	textures.OnResourceUnused(current_time, in_texture);
}

void GPUDevice::OnRenderbufferUnused(GPURenderbuffer* in_render_buffer)
{
	render_buffers.OnResourceUnused(current_time, in_render_buffer);
}





// -----------------------------------------------------------------------

void GPUBuffer::SubReference()
{
	--shared_count;
	if (shared_count == 0)
		OnLastReferenceLost();
	else if (shared_count == 1 && device != nullptr) // last reference is owned by GPUDevice
		device->OnBufferUnused(this);
}

size_t GPUBuffer::GetMemoryUsage() const
{
	return size;
}

void GPUTexture::SubReference()
{
	--shared_count;
	if (shared_count == 0)
		OnLastReferenceLost();
	else if (shared_count == 1 && device != nullptr) // last reference is owned by GPUDevice
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
	else if (shared_count == 1 && device != nullptr) // last reference is owned by GPUDevice
		device->OnRenderbufferUnused(this);
}

size_t GPURenderbuffer::GetMemoryUsage() const
{
	return 0;
}

// -----------------------------------------------------------------------

GPURenderContext::GPURenderContext(GPUDevice * in_device, chaos::Window* in_window):
	device(in_device),
	context(in_window->GetGLFWHandler())
{
	assert(device != nullptr);
	assert(context != nullptr);
}

bool GPURenderContext::IsContextActive() const
{

	return true;

	return (glfwGetCurrentContext() == context);
}

GPUFence* GPURenderContext::CreateFence()
{
	assert(IsContextActive());
	GPUFence* result = new GPUFence(this);
	if (result != nullptr)
		++fence_count;
	return result;
}

GPUQuery* GPURenderContext::CreateQuery()
{
	assert(IsContextActive());
	GPUQuery* result = new GPUQuery(this);
	if (result != nullptr)
		++query_count;
	return result;
}

GPUVertexArray* GPURenderContext::CreateVertexArray()
{
	assert(IsContextActive());
	GPUVertexArray* result = new GPUVertexArray(this);
	if (result != nullptr)
		++vertex_array_count;
	return result;
}

GPUFramebuffer* GPURenderContext::CreateFramebuffer()
{
	assert(IsContextActive());
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

	virtual bool OnDraw(chaos::GPURenderContext * render_context, chaos::GPUProgramProviderInterface const * uniform_provider, chaos::WindowDrawParams const& draw_params) override
	{


		return true;
	}

};

int main(int argc, char ** argv, char ** env)
{
	GPUDevice device;

	{
		chaos::shared_ptr<GPUBuffer> buf = device.CreateBuffer(100);

		argc = argc;
	}

	while (true)
	{
		device.Tick(1.0f);
	}


    return chaos::RunWindowApplication<WindowOpenGLTest>(argc, argv, env);
}
