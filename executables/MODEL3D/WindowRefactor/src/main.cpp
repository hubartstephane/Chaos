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

class GPURenderer;
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

protected:

	GPUBuffer(GPUDevice* in_device) :
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

protected:

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
	friend class GPURenderer;

protected:

	GPUFence(GPURenderer* in_renderer):
		renderer(in_renderer)
	{}

protected:

	GPURenderer * renderer = nullptr;

};

//---------------------------------------------------------

class GPUQuery : public GPUResource
{
	friend class GPURenderer;

protected:

	GPUQuery(GPURenderer* in_renderer) :
		renderer(in_renderer)
	{}

protected:

	GPURenderer* renderer = nullptr;
};

//---------------------------------------------------------

class GPUVertexArray : public GPUResource
{
	friend class GPURenderer;

protected:

	GPUVertexArray(GPURenderer* in_renderer) :
		renderer(in_renderer)
	{}

protected:

	GPURenderer* renderer = nullptr;
};

//---------------------------------------------------------

class GPUFramebuffer : public GPUResource
{
	friend class GPURenderer;

protected:

	GPUFramebuffer(GPURenderer* in_renderer) :
		renderer(in_renderer)
	{}

protected:

	GPURenderer* renderer = nullptr;
};





class GPUDevice // a single for the whole system
{
	friend class GPUBuffer;
	friend class GPUTexture;
	friend class GPURenderbuffer;

public:

	GPURenderer * CreateRenderer();

	GPUBuffer * CreateBuffer(size_t bufsize);

	GPUTexture* CreateTexture();

	GPURenderbuffer* CreateRenderbuffer();

protected:

	void OnBufferUnused(GPUBuffer * in_buffer);

	void OnTextureUnused(GPUTexture* in_texture);

	void OnRenderbufferUnused(GPURenderbuffer* in_renderbuffer);

protected:

	uint64_t rendering_timestamp = 0; // resources maybe shared by several GPURenderer. 
};

//---------------------------------------------------------

class GPUDrawStats
{
public:

	int draw_calls = 0;
	int vertices   = 0;
};

//---------------------------------------------------------

class GPURenderer
{
public:

	GPUFence * CreateFence();

	GPUQuery* CreateQuery();

	GPUVertexArray* CreateVertexArray();

protected:

	GPUDrawStats current_frame_stats;
	

};


// -----------------------------------------------------------------------

GPURenderer* GPUDevice::CreateRenderer()
{
	return new GPURenderer;
}

GPUBuffer* GPUDevice::CreateBuffer(size_t bufsize)
{
	return new GPUBuffer(this);
}

GPUTexture* GPUDevice::CreateTexture()
{
	return new GPUTexture(this);
}

GPURenderbuffer* GPUDevice::CreateRenderbuffer()
{
	return new GPURenderbuffer(this);
}

void GPUDevice::OnBufferUnused(GPUBuffer* inbuffer)
{
}

void GPUDevice::OnTextureUnused(GPUTexture* in_texture)
{
}

void GPUDevice::OnRenderbufferUnused(GPURenderbuffer* in_renderbuffer)
{
}

// -----------------------------------------------------------------------

void GPUBuffer::SubReference()
{
	if (--shared_count == 1)
		device->OnBufferUnused(this);
}

void GPUTexture::SubReference()
{
	if (--shared_count == 1)
		device->OnTextureUnused(this);
}

void GPURenderbuffer::SubReference()
{
	if (--shared_count == 1)
		device->OnRenderbufferUnused(this);
}

// -----------------------------------------------------------------------

GPUFence* GPURenderer::CreateFence()
{
	return new GPUFence(this);
}

GPUQuery* GPURenderer::CreateQuery()
{
	return new GPUQuery(this);
}

GPUVertexArray* GPURenderer::CreateVertexArray()
{
	return new GPUVertexArray(this);
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
