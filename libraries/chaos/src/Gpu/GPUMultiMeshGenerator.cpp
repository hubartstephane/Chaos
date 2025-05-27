#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void GPUMultiMeshGenerator::AddGenerator(GPUMeshGenerator * generator, shared_ptr<GPUMesh> & target_ptr)
	{
		assert(generator != nullptr);

		shared_ptr<GPUMeshGenerator> generator_ptr = generator;

		generators.push_back(std::make_pair(generator_ptr, &target_ptr));
	}

	void GPUMultiMeshGenerator::Clean()
	{
		generators.clear(); // destroy the intrusive_ptr
	}

	// Vertex Buffer
	//
	//           Mesh 1
	//
	//  +----+----+----+----+XXX
	//  | v0 | v1 | v2 | v3 |XXX <--- Extra padding 
	//  +----+----+----+----+XXX
	//                                   Mesh 2
	//  + - - - + - - - + - - - +-------+-------+-------+
	//  |       |       |       |   v0  |   v1  |   v2  |
	//  + - - - + - - - + - - - +-------+-------+-------+
	//
	// Mesh 1 & Mesh 2 are using the same vertex buffer
	// Mesh 2 vertices are to be aligned on their size boundaries so that they can be properly indexed
	// we add an extra padding at the end of Mesg 1 vertices

	bool GPUMultiMeshGenerator::GenerateMeshes(GPUDevice * in_gpu_device) const
	{
		if (generators.size() == 0)
			return true;

		// compute generators requirements & vertex declarations
		struct GeneratorData
		{
			GPUMeshGenerationRequirement requirement;

			shared_ptr<GPUVertexDeclaration> vertex_declaration;

			size_t vertex_size   = 0;
			size_t extra_padding = 0;
			size_t vertex_start  = 0;
		};

		std::vector<GeneratorData> generator_data;

		size_t total_vertex_buffer_size = 0;
		size_t total_index_buffer_size  = 0;
		for (auto const it : generators)
		{
			GPUMeshGenerationRequirement requirement = it.first->GetRequirement();
			if (!requirement.IsValid())
				return false;

			shared_ptr<GPUVertexDeclaration> vertex_declaration = it.first->GenerateVertexDeclaration();
			if (vertex_declaration == nullptr)
				return false;

			size_t vertex_size = vertex_declaration->GetVertexSize();

			// Are vertices properly aligned ? if not, we have to add some extra padding with previous mesh
			size_t extra_padding = (total_vertex_buffer_size % vertex_size == 0)? 
			           0:
			           vertex_size - (total_vertex_buffer_size % vertex_size);

			// insert generator data
			GeneratorData data;
			data.requirement        = requirement;
			data.vertex_declaration = vertex_declaration;
			data.vertex_size        = vertex_size;
			data.extra_padding      = extra_padding;
			data.vertex_start       = (total_vertex_buffer_size + extra_padding) / vertex_size;

			generator_data.push_back(data);

			// next mesh
			total_vertex_buffer_size += requirement.vertices_count * vertex_size + extra_padding;
			total_index_buffer_size  += requirement.indices_count * sizeof(uint32_t);
		}

		// create and map vertex and index buffer
		GPUVertexAndIndexMappedBuffers buffers = GPUVertexAndIndexMappedBuffers::CreateMappedBuffers(in_gpu_device, total_vertex_buffer_size, total_index_buffer_size);
		if (!buffers.IsValid())
			return false;

		// generate the indices and the vertices
		MemoryBufferWriter vertices_writer(buffers.mapped_vertex_buffer, total_vertex_buffer_size);
		MemoryBufferWriter indices_writer(buffers.mapped_index_buffer, total_index_buffer_size);

		size_t vertex_buffer_offset = 0;
		size_t generator_index = 0;

		for (auto const it : generators)
		{
			// create the mesh
			shared_ptr<GPUMesh> mesh = (*it.second);
			if (mesh == nullptr)
			{
				mesh = new GPUMesh(in_gpu_device); // generate the mesh
				if (mesh == nullptr)
					return false;
			}
			else
				mesh->Clear();

			// generate geometry
			GeneratorData const & data = generator_data[generator_index];

			vertices_writer.Write(nullptr, data.extra_padding); // add extra padding with previous mesh

			auto elem_create_func = [&mesh, &data, &buffers, vertex_buffer_offset]() -> GPUMeshElement&
			{
				GPUMeshElement& result = mesh->AddMeshElement(data.vertex_declaration.get(), buffers.vertex_buffer.get(), buffers.index_buffer.get());
				result.vertex_buffer_offset += vertex_buffer_offset;
				return result;
			};

			size_t written_vertices_count = vertices_writer.GetWrittenCount();
			size_t written_indices_count = indices_writer.GetWrittenCount();

			box3 bounding_box;
			it.first->GenerateMeshData(elem_create_func, vertices_writer, indices_writer, bounding_box);
			mesh->SetBoundingBox(bounding_box);

#if _DEBUG
			size_t written_vertices_count_after = vertices_writer.GetWrittenCount();
			size_t written_indices_count_after = indices_writer.GetWrittenCount();

			assert(written_vertices_count_after - written_vertices_count == data.requirement.vertices_count * data.vertex_declaration->GetVertexSize()); // ensure generator as consumed exactly all required bytes for vertices and indices
			assert(written_indices_count_after - written_indices_count == data.requirement.indices_count * sizeof(std::uint32_t));
#endif

			// shift the position of indices for this mesh
			int index_buffer_offset = (int)(written_indices_count / sizeof(uint32_t));
			if (index_buffer_offset > 0 || data.vertex_start > 0)
			{
				for (size_t i = 0 ; i < mesh->GetMeshElementCount() ; ++i)
				{
					for (GPUDrawPrimitive& primitive : mesh->GetMeshElement(i).primitives)
					{
						if (primitive.indexed)
						{
							primitive.start += index_buffer_offset;
							primitive.base_vertex_index = (int)data.vertex_start;
						}
						else
						{
							primitive.start += (int)data.vertex_start;

						}
					}
				}
			}

			(*it.second) = mesh; // store the mesh as an output

			++generator_index;
		}

		assert(vertices_writer.GetRemainingBufferSize() == 0); // ensure generators have consumed exactly what they required
		assert(indices_writer.GetRemainingBufferSize() == 0);

		// unmap buffers
		buffers.CleanResources();

		return true;
	}

}; // namespace chaos
