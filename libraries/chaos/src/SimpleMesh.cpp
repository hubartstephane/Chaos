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

		declaration.Clear();
		primitives.clear();

		vertex_array_info.clear();
		return true;
	}

	VertexArray const * SimpleMesh::GetOrCreateVertexArrayForProgram(GPUProgram const * program) const
	{
		GLuint program_id = program->GetResourceID();

		// search wether a vertex_array for that program exists
		size_t i = 0;
		size_t count = vertex_array_info.size();
		for (; i < count; ++i)
			if (vertex_array_info[i].program->GetResourceID() == program_id)
				break;

		// create a new vertex array if necessary or bind an existing one
		if (i == count)
		{
			VertexArrayInfo info;
			if (!GLTools::GenerateVertexAndIndexBuffersObject(&info.vertex_array, nullptr, nullptr))
				return nullptr;

			info.program = program;
			vertex_array_info.push_back(info);

			GPUProgramData const & data = program->GetProgramData();

			GLuint va = vertex_array_info[i].vertex_array->GetResourceID();
			data.BindAttributes(va, declaration, nullptr);

			if (index_buffer != nullptr)
				glVertexArrayElementBuffer(va, index_buffer->GetResourceID());

			if (vertex_buffer != nullptr)  // simple mesh only use one vertex_buffer : binding_index is always 0
			{
				GLuint binding_index = 0;
				glVertexArrayVertexBuffer(va, binding_index, vertex_buffer->GetResourceID(), vertex_buffer_offset, declaration.GetVertexSize());
			}
		}
		return vertex_array_info[i].vertex_array.get();
	}

	void SimpleMesh::Render(GPUProgram const * program, GPUProgramProviderBase const * uniform_provider, int instance_count, int base_instance) const
	{
		assert(program != nullptr);
		// find the vertex array to use
		VertexArray const * vertex_array = GetOrCreateVertexArrayForProgram(program);
		if (vertex_array == nullptr)
			return;
		// use program and bind uniforms
		program->UseProgram(uniform_provider);
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// render the primitives
		for (auto const & primitive : primitives)
			primitive.Render(instance_count, base_instance);
	}

	void SimpleMesh::Render(RenderMaterial const * material, GPUProgramProviderBase const * uniform_provider, int instance_count, int base_instance) const
	{
		assert(material != nullptr);
		// get the program for the material
		GPUProgram const * program = material->GetEffectiveProgram();
		if (program == nullptr)
			return;
		// find the vertex array to use
		VertexArray const * vertex_array = GetOrCreateVertexArrayForProgram(program);
		if (vertex_array == nullptr)
			return;
		// use material and bind uniforms
		material->UseMaterial(uniform_provider);
		// bind the vertex array
		glBindVertexArray(vertex_array->GetResourceID());
		// render the primitives
		for (auto const & primitive : primitives)
			primitive.Render(instance_count, base_instance);
	}


	void SimpleMesh::SetVertexBufferOffset(GLintptr in_vertex_buffer_offset)
	{
		vertex_buffer_offset = in_vertex_buffer_offset;
	}

}; // namespace chaos
