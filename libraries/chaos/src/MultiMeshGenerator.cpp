#include <chaos/MultiMeshGenerator.h>
#include <chaos/GLTools.h>

namespace chaos
{

	MultiMeshGenerator::MultiMeshGenerator()
	{
	}

	MultiMeshGenerator::~MultiMeshGenerator()
	{
		Clean();
	}

	void MultiMeshGenerator::AddGenerator(SimpleMeshGenerator * generator, shared_ptr<SimpleMesh> & target_ptr)
	{
		assert(generator != nullptr);

		shared_ptr<SimpleMeshGenerator> generator_ptr = generator;

		generators.push_back(std::make_pair(generator_ptr, &target_ptr));
	}

	void MultiMeshGenerator::Clean()
	{
		generators.clear(); // destroy the intrusive_ptr
	}

	bool MultiMeshGenerator::GenerateMeshes() const
	{
		if (generators.size() == 0)
			return true;

		// compute the final requirements
		int vb_size = 0;
		int ib_size = 0;

		for (auto const it : generators)
		{
			MeshGenerationRequirement requirement = it.first->GetRequirement();
			if (!requirement.IsValid())
				return false;

			vb_size += requirement.vertex_size * requirement.vertices_count;
			ib_size += requirement.indices_count * sizeof(GLuint);
		}

		// create a vertex buffer and an index buffer to be shared among all meshes
		shared_ptr<GPUBuffer> vertex_buffer;
		shared_ptr<GPUBuffer>  index_buffer;

		shared_ptr<GPUBuffer> * vb_ptr = (vb_size > 0) ? &vertex_buffer : nullptr;
		shared_ptr<GPUBuffer>  * ib_ptr = (ib_size > 0) ? &index_buffer : nullptr;

		if (GLTools::GenerateVertexAndIndexBuffers(nullptr, vb_ptr, ib_ptr, false, false)) // vertex_buffer and index_buffer are static
		{
			GLuint vb = (vb_size > 0) ? vertex_buffer->GetResourceID() : 0;
			GLuint ib = (ib_size > 0) ? index_buffer->GetResourceID() : 0;

			// allocate buffer for vertices and indices
			std::pair<char *, GLuint *> mapping;
			if (GLTools::MapBuffers(vb, ib, vb_size, ib_size, mapping)) // shuxxx
			{
				// generate the indices and the vertices
				MemoryBufferWriter vertices_writer(mapping.first, vb_size);
				MemoryBufferWriter indices_writer(mapping.second, ib_size);

				for (auto const it : generators)
				{
					MeshGenerationRequirement requirement = it.first->GetRequirement();

					size_t written_vertices_count = vertices_writer.GetWrittenCount();
					size_t written_indices_count = indices_writer.GetWrittenCount();

					shared_ptr<SimpleMesh> mesh = (*it.second);
					if (mesh == nullptr)
						mesh = new SimpleMesh; // generate the mesh
					else
						mesh->Release(); // reuse existing mesh

					if (mesh != nullptr)
					{
						if (requirement.vertices_count > 0)
							mesh->vertex_buffer = vertex_buffer;
						if (requirement.indices_count > 0)
							mesh->index_buffer = index_buffer;

#if _DEBUG
						size_t vc1 = vertices_writer.GetWrittenCount();
						size_t ic1 = indices_writer.GetWrittenCount();
#endif

						it.first->GenerateMeshData(mesh->primitives, vertices_writer, indices_writer); // generate the buffers and primitives and declaration
						it.first->GenerateVertexDeclaration(mesh->vertex_declaration);

#if _DEBUG
						size_t vc2 = vertices_writer.GetWrittenCount();
						size_t ic2 = indices_writer.GetWrittenCount();

						auto ppp = vc2 - vc1;
						auto uuu = requirement.vertices_count * requirement.vertex_size;

						assert(vc2 - vc1 == requirement.vertices_count * requirement.vertex_size);
#endif

						assert(requirement.vertex_size == mesh->vertex_declaration.GetVertexSize());

						mesh->ShiftPrimitivesIndexAndVertexPosition(0, (int)(written_indices_count / sizeof(GLuint)));  // shift the position of vertices/indices for this mesh

						mesh->SetVertexBufferOffset(written_vertices_count);

						(*it.second) = mesh; // store the mesh as an output          
					}
				}

				auto ppp = vertices_writer.GetRemainingBufferSize();

				assert(vertices_writer.GetRemainingBufferSize() == 0);
				assert(indices_writer.GetRemainingBufferSize() == 0);

				// transfert data to GPU and free memory
				if (vb != 0)
					glUnmapNamedBuffer(vb);
				if (ib != 0)
					glUnmapNamedBuffer(ib);
			}
		}
		return true;
	}

}; // namespace chaos
