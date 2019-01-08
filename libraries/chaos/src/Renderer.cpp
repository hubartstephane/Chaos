#include <chaos/Renderer.h>

namespace chaos
{

	void Renderer::BeginRenderingFrame()
	{

	}

	void Renderer::EndRenderingFrame()
	{
		// update the frame rate
		framerate_counter.Accumulate(1.0f);
		// increment the timestamp
		++rendering_timestamp;
	}

	float Renderer::GetFrameRate() const 
	{ 
		return framerate_counter.GetCurrentValue();
	}

	uint64_t Renderer::GetTimestamp() const 
	{ 
		return rendering_timestamp; 
	}

	bool Renderer::DoTick(double delta_time)
	{
		// update counters
		framerate_counter.Tick((float)delta_time);
		drawcall_counter.Tick((float)delta_time);
		vertices_counter.Tick((float)delta_time);

		return true;
	}

	void Renderer::Draw(DrawPrimitive const & primitive, InstancingInfo const & instancing)
	{
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

}; // namespace chaos
