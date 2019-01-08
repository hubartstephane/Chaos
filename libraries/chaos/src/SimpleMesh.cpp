#include <chaos/SimpleMesh.h>
#include <chaos/GLTools.h>

namespace chaos
{
	SimpleMesh::~SimpleMesh()
	{
		DoRelease();
	}

	void SimpleMesh::ShiftPrimitivesIndexAndVertexPosition(int vb_offset, int ib_offset)
	{
		if (vb_offset != 0 || ib_offset != 0)
			for (auto & primitive : primitives)
				primitive.ShiftIndexAndVertexPosition(vb_offset, ib_offset);
	}

	bool SimpleMesh::DoRelease()
	{
		vertex_buffer = nullptr;
		index_buffer = nullptr;

		vertex_declaration.Clear();
		primitives.clear();

		vertex_array_cache.Clear();
		return true;
	}

	void SimpleMesh::Render(Renderer * renderer, GPUProgram const * program, GPUProgramProviderBase const * uniform_provider,InstancingInfo const & instancing) const
	{
		// early exit
		if (program == nullptr)
			return;
		// use program
		program->UseProgram(uniform_provider);
		// do the rendering
		DoRender(renderer, program, uniform_provider, instancing);
	}

	void SimpleMesh::Render(Renderer * renderer, GPURenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		// early exit
		if (material == nullptr)
			return;
		// get the program for the material
		GPUProgram const * program = material->GetEffectiveProgram();
		if (program == nullptr)
			return;
		// use material and bind uniforms
		material->UseMaterial(uniform_provider);
		// do the rendering
		DoRender(renderer, program, uniform_provider, instancing);
	}

	void SimpleMesh::DoRender(Renderer * renderer, GPUProgram const * program, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		assert(program != nullptr);
		// find the vertex array to use
		GPUVertexArray const * vertex_array = vertex_array_cache.FindOrCreateVertexArray(program, vertex_buffer.get(), index_buffer.get(), vertex_declaration, vertex_buffer_offset);
		if (vertex_array == nullptr)
			return;
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// render the primitives
		for (auto const & primitive : primitives)
			renderer->Draw(primitive, instancing);
		// unbind the vertex array
		glBindVertexArray(0);
	}

	void SimpleMesh::SetVertexBufferOffset(GLintptr in_vertex_buffer_offset)
	{
		vertex_buffer_offset = in_vertex_buffer_offset;
	}

}; // namespace chaos
