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

	VertexArray const * SimpleMesh::GetOrCreateVertexArrayForProgram(GPUProgram const * program) const
	{
		return vertex_array_cache.FindOrCreateVertexArray(program, vertex_buffer.get(), index_buffer.get(), vertex_declaration);
	}

	void SimpleMesh::Render(GPUProgram const * program, GPUProgramProviderBase const * uniform_provider,InstancingInfo const & instancing) const
	{
		// early exit
		if (program == nullptr)
			return;
		// use program
		program->UseProgram(uniform_provider);
		// do the rendering
		DoRender(program, uniform_provider, instancing);
	}

	void SimpleMesh::Render(RenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
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
		DoRender(program, uniform_provider, instancing);
	}

	void SimpleMesh::DoRender(GPUProgram const * program, GPUProgramProviderBase const * uniform_provider, InstancingInfo const & instancing) const
	{
		assert(program != nullptr);
		// find the vertex array to use
		VertexArray const * vertex_array = GetOrCreateVertexArrayForProgram(program);
		if (vertex_array == nullptr)
			return;
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// render the primitives
		for (auto const & primitive : primitives)
			primitive.Render(instancing);
	}

	void SimpleMesh::SetVertexBufferOffset(GLintptr in_vertex_buffer_offset)
	{
		vertex_buffer_offset = in_vertex_buffer_offset;
	}

}; // namespace chaos
