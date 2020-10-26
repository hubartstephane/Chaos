#include <chaos/Chaos.h>

namespace chaos
{

	bool GPURenderer::PushFramebufferRenderContext(GPUFramebuffer * framebuffer, bool generate_mipmaps)
	{
#if _DEBUG 
		assert(rendering_started);
#endif 
		// check parameter validity
		if (framebuffer == nullptr || !framebuffer->IsValid())
			return false;
		// search whether the framebuffer is already on the stack
		for (GPUFramebufferRenderData const & frd : framebuffer_stack)
			if (frd.framebuffer == framebuffer)
				return false;
		// push the context on the stack
		GPUFramebufferRenderData frd;
		frd.framebuffer = framebuffer;
		frd.generate_mipmaps = generate_mipmaps;
		glGetIntegerv(GL_VIEWPORT, frd.viewport_to_restore);
		framebuffer_stack.push_back(frd);
		// update GL state machine
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetResourceID());
		return true;
	}
	
	bool GPURenderer::PopFramebufferRenderContext()
	{
#if _DEBUG 
		assert(rendering_started);
#endif
		assert(framebuffer_stack.size() > 0); // logic error 

		// check for release version
		if (framebuffer_stack.size() == 0)
			return false;
		// remove the element of the stack
		GPUFramebufferRenderData frd = framebuffer_stack[framebuffer_stack.size() - 1];
		framebuffer_stack.pop_back();
		// check the framebuffer
		if (!frd.framebuffer->IsValid())
			return false;
		// generate mipmaps
		if (frd.generate_mipmaps)
		{
			for (GPUFramebufferAttachmentInfo & info : frd.framebuffer->attachment_info)
				if (info.texture != nullptr)
					glGenerateTextureMipmap(info.texture->GetResourceID());
		}
		// update GL state machine
		GPUFramebuffer * previous_framebuffer = nullptr;
		if (framebuffer_stack.size() > 0 && framebuffer_stack[framebuffer_stack.size() - 1].framebuffer->IsValid())
			previous_framebuffer = framebuffer_stack[framebuffer_stack.size() - 1].framebuffer.get();

		glBindFramebuffer(GL_FRAMEBUFFER, (previous_framebuffer == nullptr)? 0 : previous_framebuffer->GetResourceID()); 

		glViewport(frd.viewport_to_restore[0], frd.viewport_to_restore[1], frd.viewport_to_restore[2], frd.viewport_to_restore[3]);

		return true;
	}

	void GPURenderer::BeginRenderingFrame()
	{
#if _DEBUG 
		assert(!rendering_started);
		rendering_started = true;
#endif

		// increment the timestamp
		++rendering_timestamp;
		// unreference the fence (users of this fence must have a reference on it)
		rendering_fence = nullptr;
	}

	void GPURenderer::EndRenderingFrame()
	{
#if _DEBUG 
		assert(rendering_started);
		rendering_started = false;
#endif
		assert(framebuffer_stack.size() == 0); // logic error 
		// in release, pop all previous context
		while(framebuffer_stack.size() > 0)
			PopFramebufferRenderContext();

		// update the frame rate
		framerate_counter.Accumulate(1.0f);
		// push the frame fence in the command queue if required by some external users
		if (rendering_fence != nullptr)
			rendering_fence->CreateGPUFence();
	}

	float GPURenderer::GetFrameRate() const 
	{ 
		return framerate_counter.GetCurrentValue();
	}

	GPUFence * GPURenderer::GetCurrentFrameFence()
	{
		if (rendering_fence == nullptr)
			rendering_fence = new GPUFence(nullptr); // does not create the OPENGL resource. It will be created at the end of frame
		return rendering_fence.get();
	}

	uint64_t GPURenderer::GetTimestamp() const 
	{ 
		return rendering_timestamp; 
	}

	bool GPURenderer::DoTick(float delta_time)
	{
		// update counters
		framerate_counter.Tick(delta_time);
		drawcall_counter.Tick(delta_time);
		vertices_counter.Tick(delta_time);

		return true;
	}

	void GPURenderer::Draw(GPUDrawPrimitive const & primitive, GPUInstancingInfo const & instancing)
	{
#if _DEBUG 
		assert(rendering_started);
#endif

		// This function is able to render : 
		//   -normal primitives
		//   -indexed primitives
		//   -instanced primitives
		//
		// We do not use yet :
		//   -indirect primitive
		//   -multi draws possibilities

		if (primitive.count <= 0)
			return;

		if (!primitive.indexed)
		{
			if (instancing.instance_count <= 1)
			{
				glDrawArrays(primitive.primitive_type, primitive.start, primitive.count);
			}
			else
			{
				if (instancing.base_instance == 0)
					glDrawArraysInstanced(primitive.primitive_type, primitive.start, primitive.count, instancing.instance_count);
				else
					glDrawArraysInstancedBaseInstance(primitive.primitive_type, primitive.start, primitive.count, instancing.instance_count, instancing.base_instance);
			}
		}
		else
		{
			GLvoid * offset = ((int32_t*)nullptr) + primitive.start;
			if (instancing.instance_count <= 1)
			{
				if (primitive.base_vertex_index == 0)
					glDrawElements(primitive.primitive_type, primitive.count, GL_UNSIGNED_INT, offset);
				else
					glDrawElementsBaseVertex(primitive.primitive_type, primitive.count, GL_UNSIGNED_INT, offset, primitive.base_vertex_index);
			}
			else
			{
				if (primitive.base_vertex_index == 0)
					glDrawElementsInstanced(primitive.primitive_type, primitive.count, GL_UNSIGNED_INT, offset, instancing.instance_count);
				else
					glDrawElementsInstancedBaseVertexBaseInstance(primitive.primitive_type, primitive.count, GL_UNSIGNED_INT, offset, instancing.instance_count, primitive.base_vertex_index, instancing.base_instance);
			}
		}

		// update some statistics
		int instance_count = (instancing.instance_count > 1) ? instancing.instance_count : 1;
		vertices_counter.Accumulate((float)primitive.count * instance_count);

		drawcall_counter.Accumulate(1.0f);		
	}

	bool GPURenderer::Initialize()
	{
		GPUMultiMeshGenerator generators;

		box2 box = box2(glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f));
		generators.AddGenerator(new chaos::GPUQuadMeshGenerator(box), quad_mesh);

		if (!generators.GenerateMeshes())
			return false;

		return true;
	}

	void GPURenderer::DrawFullscreenQuad(GPURenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, GPURenderParams const & render_params)
	{
		assert(material != nullptr);
		if (quad_mesh != nullptr)
			quad_mesh->Render(this, material, uniform_provider, render_params);
	}

    GPUBuffer * GPURenderer::GetQuadIndexBuffer(size_t * result_quad_count)
    {
        size_t const QUAD_COUNT = 10000;

        if (quad_index_buffer == nullptr)
        {
            quad_index_buffer = new GPUBuffer(true);
            if (quad_index_buffer != nullptr)
            {                
                size_t buffer_size = 6 * QUAD_COUNT * sizeof(GLuint); // quad becomes a triangle pair
                // reserve a buffer
                quad_index_buffer->SetBufferData(nullptr, buffer_size);
                // fill the buffer
                GLuint * buffer = (GLuint *)quad_index_buffer->MapBuffer(0, 0, false, true);
                if (buffer != nullptr)
                {
                    for (size_t i = 0; i < QUAD_COUNT; ++i)
                    {
                        size_t start = i * 4;

                        buffer[0] = (GLuint)(start + 0);
                        buffer[1] = (GLuint)(start + 1);
                        buffer[2] = (GLuint)(start + 2);
                        buffer[3] = (GLuint)(start + 3);
                        buffer[4] = (GLuint)(start + 0);
                        buffer[5] = (GLuint)(start + 2);
                        buffer += 6;
                    }
                }
                quad_index_buffer->UnMapBuffer();
            }
        }

        if (result_quad_count != nullptr)
            *result_quad_count = QUAD_COUNT;

        return quad_index_buffer.get();
    }

}; // namespace chaos
