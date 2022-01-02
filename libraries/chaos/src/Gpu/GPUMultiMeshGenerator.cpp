#include <chaos/Chaos.h>

namespace chaos
{

	GPUMultiMeshGenerator::GPUMultiMeshGenerator()
	{
	}

	GPUMultiMeshGenerator::~GPUMultiMeshGenerator()
	{
		Clean();
	}

	void GPUMultiMeshGenerator::AddGenerator(GPUMeshGenerator * generator, shared_ptr<GPUDynamicMesh> & target_ptr)
	{
		assert(generator != nullptr);

		shared_ptr<GPUMeshGenerator> generator_ptr = generator;

		generators.push_back(std::make_pair(generator_ptr, &target_ptr));
	}

	void GPUMultiMeshGenerator::Clean()
	{
		generators.clear(); // destroy the intrusive_ptr
	}

	bool GPUMultiMeshGenerator::GenerateMeshes() const
	{
		if (generators.size() == 0)
			return true;

		// compute the final requirements
		int vb_size = 0;
		int ib_size = 0;

		for (auto const it : generators)
		{
			GPUMeshGenerationRequirement requirement = it.first->GetRequirement();
			if (!requirement.IsValid())
				return false;

			vb_size += requirement.vertex_size * requirement.vertices_count;
			ib_size += requirement.indices_count * sizeof(GLuint);
		}

		// create a vertex buffer to be shared among all meshes
		shared_ptr<GPUBuffer> vertex_buffer;
		if (vb_size > 0)
		{
			vertex_buffer = new GPUBuffer(false);
			if (vertex_buffer == nullptr || !vertex_buffer->IsValid())
				return false;
			vertex_buffer->SetBufferData(nullptr, vb_size);
		}

		// create a index buffer to be shared among all meshes
		shared_ptr<GPUBuffer> index_buffer;
		if (ib_size > 0)
		{
			index_buffer = new GPUBuffer(false);
			if (index_buffer == nullptr || !index_buffer->IsValid())
				return false;
			index_buffer->SetBufferData(nullptr, ib_size);
		}

		// map the buffers		
		char* vb_ptr = (vertex_buffer != nullptr)? vertex_buffer->MapBuffer(0, 0, false, true) : nullptr;
		char* ib_ptr = (index_buffer != nullptr)?  index_buffer->MapBuffer(0, 0, false, true) : nullptr;

		// generate the indices and the vertices
		MemoryBufferWriter vertices_writer(vb_ptr, vb_size);
		MemoryBufferWriter indices_writer(ib_ptr, ib_size);

		for (auto const it : generators)
		{
			GPUMeshGenerationRequirement requirement = it.first->GetRequirement();

			size_t written_vertices_count = vertices_writer.GetWrittenCount();
			size_t written_indices_count = indices_writer.GetWrittenCount();

			shared_ptr<GPUDynamicMesh> mesh = (*it.second);
			if (mesh == nullptr)
			{
				mesh = new GPUDynamicMesh; // generate the mesh
				if (mesh == nullptr)
					continue;
			}
			else
				mesh->Clear(nullptr); // reuse existing mesh

			GPUDynamicMeshElement& element = mesh->AddMeshElement();

			if (requirement.vertices_count > 0)
				element.vertex_buffer = vertex_buffer;
			if (requirement.indices_count > 0)
				element.index_buffer = index_buffer;
#if _DEBUG
			size_t vc1 = vertices_writer.GetWrittenCount();
			size_t ic1 = indices_writer.GetWrittenCount();
#endif
			it.first->GenerateMeshData(element.primitives, vertices_writer, indices_writer); // generate the buffers and primitives and declaration
			element.vertex_declaration = it.first->GenerateVertexDeclaration();

#if _DEBUG
			size_t vc2 = vertices_writer.GetWrittenCount();
			size_t ic2 = indices_writer.GetWrittenCount();

			assert(vc2 - vc1 == requirement.vertices_count * requirement.vertex_size);
			assert(ic2 - ic1 == requirement.indices_count * sizeof(std::uint32_t));
#endif

			assert(requirement.vertex_size == element.vertex_declaration->GetVertexSize());

			//mesh->ShiftPrimitivesIndexAndVertexPosition(0, (int)(written_indices_count / sizeof(GLuint)));  // shift the position of vertices/indices for this mesh

			//mesh->SetVertexBufferOffset(written_vertices_count);

			(*it.second) = mesh; // store the mesh as an output          
		}

		assert(vertices_writer.GetRemainingBufferSize() == 0);
		assert(indices_writer.GetRemainingBufferSize() == 0);

		// unmap buffers
		if (vertex_buffer != nullptr)
			vertex_buffer->UnMapBuffer();
		if (index_buffer != nullptr)
			index_buffer->UnMapBuffer();

		return true;
	}

}; // namespace chaos
