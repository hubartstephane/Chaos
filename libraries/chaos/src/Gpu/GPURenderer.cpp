#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	GPURenderer::GPURenderer(Window* in_window) :
		window(in_window)
	{
		assert(in_window != nullptr);
	}

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

		GLint viewport_data[4];
		glGetIntegerv(GL_VIEWPORT, viewport_data);
		frd.viewport_to_restore.position = { viewport_data[0], viewport_data[1] };
		frd.viewport_to_restore.size = { viewport_data[2], viewport_data[3] };

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

		GLTools::SetViewport(frd.viewport_to_restore);

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

	float GPURenderer::GetAverageFrameRate() const
	{
		return framerate_counter.GetCurrentValue();
	}

	int GPURenderer::GetAverageDrawCalls() const
	{
		return drawcall_counter.GetCurrentValue();
	}

	int GPURenderer::GetAverageVertices() const
	{
		return vertices_counter.GetCurrentValue();
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
		LastDeltaTime = delta_time;
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
		vertices_counter.Accumulate(primitive.count * instance_count);

		drawcall_counter.Accumulate(1);
	}

	void GPURenderer::DrawFullscreenQuad(GPURenderMaterial const * material, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
	{
		assert(material != nullptr);

		GPUResourceManager* gpu_resource_manager = WindowApplication::GetGPUResourceManagerInstance();
		if (gpu_resource_manager != nullptr)
		{
			GPUMesh* quad_mesh = gpu_resource_manager->GetQuadMesh();
			if (quad_mesh != nullptr)
				quad_mesh->DisplayWithMaterial(material, this, uniform_provider, render_params);
		}
	}

}; // namespace chaos
