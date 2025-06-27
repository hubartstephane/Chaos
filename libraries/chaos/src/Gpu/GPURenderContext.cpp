#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{

	CHAOS_GLOBAL_VARIABLE(size_t, renderer_stats_size, 1200 * 20) // let's say at 1200 fps for very small project, we have 20 seconds stored

	GPURenderContext::GPURenderContext(GPUDevice* in_gpu_device, Window* in_window) :
		GPUDeviceResourceInterface(in_gpu_device),
		window(in_window),
		stats(renderer_stats_size.Get()),
		vertex_array_cache(this)
	{
		assert(in_window != nullptr);
	}

	bool GPURenderContext::BindVertexArray(GPUVertexArrayBindingInfo const & binding_info)
	{
		//assert(IsRenderContextCurrent());

		// gets the vertex array
		GPUVertexArray const* vertex_array = vertex_array_cache.FindOrCreateVertexArray(binding_info);
		if (vertex_array == nullptr)
			return false;

		GLuint vertex_array_id = vertex_array->GetResourceID();
		glBindVertexArray(vertex_array_id);
		return true;
	}

	void GPURenderContext::UnbindVertexArray()
	{
		//assert(IsRenderContextCurrent());
		glBindVertexArray(0);
	}

	void GPURenderContext::Destroy()
	{
		vertex_array_cache.Destroy();
	}

	bool GPURenderContext::RenderIntoFramebuffer(GPUFramebuffer* framebuffer, bool generate_mipmaps, LightweightFunction<bool()> render_func)
	{	
		// early exit
		if (framebuffer == nullptr || !framebuffer->IsValid())
			return false;

		if (framebuffer->IsRenderingInProgress())
			return false;

		// start rendering
		++offscreen_rendering_count;

		// notify the framebuffer
		framebuffer->SetRenderingInProgress(true);

		// save some states
		GLint viewport_data[4];
		glGetIntegerv(GL_VIEWPORT, viewport_data);

		bool scissor_test = glIsEnabled(GL_SCISSOR_TEST);

		GLint scissor_data[4];
		glGetIntegerv(GL_SCISSOR_BOX, scissor_data);

		GLint previous_framebuffer_id = 0;
		glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previous_framebuffer_id);

		// update GL state machine
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetResourceID());
		GLTools::SetViewport(framebuffer->GetBox());
		glDisable(GL_SCISSOR_TEST);

		// render
		bool result = render_func();

		// generate mipmap
		if (result)
			if (generate_mipmaps)
				for (GPUFramebufferAttachmentInfo& info : framebuffer->attachment_info)
					if (info.texture != nullptr)
						glGenerateTextureMipmap(info.texture->GetResourceID());

		// restore some states
		glViewport(viewport_data[0], viewport_data[1], viewport_data[2], viewport_data[3]);

		if (scissor_test)
			glEnable(GL_SCISSOR_TEST);
		else 
			glDisable(GL_SCISSOR_TEST);

		glScissor(scissor_data[0], scissor_data[1], scissor_data[2], scissor_data[3]);

		glBindFramebuffer(GL_FRAMEBUFFER, previous_framebuffer_id);

		// notify the framebuffer
		framebuffer->SetRenderingInProgress(false);

		// end rendering
		--offscreen_rendering_count;

		return true;
	}

	bool GPURenderContext::RenderFrame(LightweightFunction<bool()> render_func)
	{
		if (rendering_started || offscreen_rendering_count > 0)
		{
			GPURenderContextLog::Error("GPURenderContext::RenderFrame: no reentrance");
			return false;
		}
		rendering_started = true;

		BeginRenderingFrame();
		bool result = render_func();
		EndRenderingFrame();

		rendering_started = false;

		return result;
	}

	void GPURenderContext::BeginRenderingFrame()
	{
		// prepare stats
		current_frame_stat = GPURenderContextFrameStats();
		current_frame_stat.rendering_timestamp = ++rendering_timestamp;
		current_frame_stat.frame_start_time = (float)glfwGetTime();
	}

	void GPURenderContext::EndRenderingFrame()
	{
		// update the frame rate
		framerate_counter.Accumulate(1.0f);
		// store statistics
		current_frame_stat.frame_end_time = (float)glfwGetTime();
		current_frame_stat.rendering_timestamp = rendering_timestamp;
		stats.push_back(current_frame_stat);
		// prepare next frame stats
		current_frame_stat = GPURenderContextFrameStats();
	}

	float GPURenderContext::GetAverageFrameRate() const
	{
		return framerate_counter.GetCurrentValue();
	}

	int GPURenderContext::GetAverageDrawCalls() const
	{
		return drawcall_counter.GetCurrentValue();
	}

	int GPURenderContext::GetAverageVertices() const
	{
		return vertices_counter.GetCurrentValue();
	}

	uint64_t GPURenderContext::GetTimestamp() const
	{
		return rendering_timestamp;
	}

	bool GPURenderContext::DoTick(float delta_time)
	{
		last_delta_time = delta_time;
		
		vertex_array_cache.Tick(delta_time);

		// update counters
		framerate_counter.Tick(delta_time);
		drawcall_counter.Tick(delta_time);
		vertices_counter.Tick(delta_time);

		return true;
	}

	void GPURenderContext::Draw(GPUDrawPrimitive const & primitive, GPUInstancingInfo const & instancing)
	{
		assert(rendering_started || offscreen_rendering_count > 0);

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
		current_frame_stat.vertices_counter += primitive.count * instance_count;

		drawcall_counter.Accumulate(1);
		++current_frame_stat.drawcall_counter;
	}

	void GPURenderContext::DrawFullscreenQuad(GPURenderMaterial const * material, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const & render_params)
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

	void GPURenderContext::OnBufferDestroyed(GLuint in_buffer_id)
	{
		vertex_array_cache.OnBufferDestroyed(in_buffer_id);
	}

	void GPURenderContext::OnProgramDestroyed(GLuint in_program_id)
	{
		vertex_array_cache.OnProgramDestroyed(in_program_id);
	}

	void GPURenderContext::SubReference()
	{
		--shared_count;
		if (shared_count == 0)
			OnLastReferenceLost();
		else if (shared_count == 1) // the last reference is comming from GPUDevice
			gpu_device->OnRenderContextDestroyed(this);
	}

	bool GPURenderContext::IsRenderContextCurrent() const
	{
		if (window == nullptr)
			return false;
		return (glfwGetCurrentContext() == window->GetGLFWHandler());
	}

}; // namespace chaos
