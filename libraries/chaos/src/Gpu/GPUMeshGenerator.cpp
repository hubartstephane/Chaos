#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

// OpenGL coordinates for a fullscreen representation
//   -1          +1
// +1 +----------+
//    |          |
//    |          |
//    |          |
//    |          |
// -1 +----------+
//

namespace chaos
{
	// =====================================================================
	// GPUMeshGenerationRequirement
	// =====================================================================

	bool GPUMeshGenerationRequirement::IsValid() const
	{
		if (vertices_count >= 1) // at least one vertex
			return true;
		return false;
	}

	// =====================================================================
	// GPUMeshGenerator
	// =====================================================================

	bool GPUMeshGenerator::FillMeshData(GPUMesh* mesh) const
	{
		assert(mesh != nullptr);

		// get requirement
		GPUMeshGenerationRequirement requirement = GetRequirement();
		if (!requirement.IsValid())
			return false;

		// generate a declaration
		shared_ptr<GPUVertexDeclaration> vertex_declaration = GenerateVertexDeclaration();
		if (vertex_declaration == nullptr)
			return false;

		// create a vertex buffer
		size_t vertex_size        = vertex_declaration->GetVertexSize();
		size_t vertex_buffer_size = requirement.vertices_count * vertex_size;
		size_t index_buffer_size  = requirement.indices_count * sizeof(uint32_t);

		GPUVertexAndIndexMappedBuffers buffers = GPUVertexAndIndexMappedBuffers::CreateMappedBuffers(vertex_buffer_size, index_buffer_size);
		if (!buffers.IsValid())
			return false;

		// prepare the mesh
		mesh->Clear(nullptr);

		// generate the indices and the vertices
		auto elem_create_func = [&mesh, &vertex_declaration, &buffers]() -> GPUMeshElement &
		{
			return mesh->AddMeshElement(vertex_declaration.get(), buffers.vertex_buffer.get(), buffers.index_buffer.get());
		};

		MemoryBufferWriter vertices_writer(buffers.mapped_vertex_buffer, vertex_buffer_size);
		MemoryBufferWriter indices_writer(buffers.mapped_index_buffer, index_buffer_size);
		GenerateMeshData(elem_create_func, vertices_writer, indices_writer);

		assert(vertices_writer.GetRemainingBufferSize() == 0);
		assert(indices_writer.GetRemainingBufferSize() == 0);

		// unmap buffers
		buffers.CleanResources();

		return true;
	}

	shared_ptr<GPUMesh> GPUMeshGenerator::GenerateMesh() const
	{
		shared_ptr<GPUMesh> mesh = new GPUMesh();
		if (mesh != nullptr)
		{
			if (!FillMeshData(mesh.get())) // automatic destruction in case of failure
				return nullptr;
		}
		return mesh;
	}

	// =====================================================================
	// GPUTriangleMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUTriangleMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertices_count = 3;
		result.indices_count = 0;
		return result;
	}

    GPUVertexDeclaration * GPUTriangleMeshGenerator::GenerateVertexDeclaration() const
	{
        GPUVertexDeclaration* result = new GPUVertexDeclaration();
        if (result != nullptr)
        {
            result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
            result->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);
        }
        return result;
	}

	void GPUTriangleMeshGenerator::GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		GPUMeshElement & mesh_element = elem_create_func();

		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = 3;
		draw_primitive.indexed = false;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		mesh_element.primitives.push_back(draw_primitive);

		glm::vec3 transformed_a = GLMTools::MultWithTranslation(transform, primitive.a);
		glm::vec3 transformed_b = GLMTools::MultWithTranslation(transform, primitive.b);
		glm::vec3 transformed_c = GLMTools::MultWithTranslation(transform, primitive.c);

		glm::vec3 transformed_normal = glm::normalize(glm::cross(transformed_b - transformed_a, transformed_c - transformed_b));

		vertices_writer << transformed_a;
		vertices_writer << transformed_normal;
		vertices_writer << transformed_b;
		vertices_writer << transformed_normal;
		vertices_writer << transformed_c;
		vertices_writer << transformed_normal;

		box3 bounding_box;
		ExtendBox(bounding_box, transformed_a);
		ExtendBox(bounding_box, transformed_b);
		ExtendBox(bounding_box, transformed_c);
	}

	// =====================================================================
	// GPUQuadMeshGenerator
	// =====================================================================

	glm::vec3 const GPUQuadMeshGenerator::vertices[4] =
	{
		{ -1.0f, -1.0f,  0.0f },
		{  1.0f, -1.0f,  0.0f },
		{  1.0f,  1.0f,  0.0f },
		{ -1.0f,  1.0f,  0.0f }
	};

	uint32_t const GPUQuadMeshGenerator::triangles[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	GPUMeshGenerationRequirement GPUQuadMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertices_count = sizeof(vertices) / sizeof(vertices[0]);
		result.indices_count = sizeof(triangles) / sizeof(triangles[0]);
		return result;
	}

    GPUVertexDeclaration * GPUQuadMeshGenerator::GenerateVertexDeclaration() const
	{
        GPUVertexDeclaration* result = new GPUVertexDeclaration();
        if (result != nullptr)
        {
            result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
            result->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);
        }
        return result;
	}

	void GPUQuadMeshGenerator::GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		GPUMeshElement& mesh_element = elem_create_func();

		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = sizeof(triangles) / sizeof(triangles[0]);
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		mesh_element.primitives.push_back(draw_primitive);

		// the indices
		indices_writer.Write(triangles, sizeof(triangles));

		// the vertices	
		glm::vec3 normal = {0.0f, 0.0f, 1.0f};
		glm::vec3 transformed_normal = GLMTools::Mult(transform, normal);

		box3 bounding_box;
		
		size_t const vertex_count = sizeof(vertices) / sizeof(vertices[0]);
		for (size_t i = 0; i < vertex_count; ++i)
		{
			glm::vec3 position = vertices[i] * glm::vec3(primitive.half_size, 0.0f) + glm::vec3(primitive.position, 0.0f);
			glm::vec3 transformed_position = GLMTools::MultWithTranslation(transform, position);

			vertices_writer << transformed_position;
			vertices_writer << transformed_normal;

			ExtendBox(bounding_box, transformed_position);
		}
	}

	// =====================================================================
	// GPUBoxMeshGenerator
	// =====================================================================

	glm::vec3 const GPUBoxMeshGenerator::vertices[24 * 2] = // position + normal
	{
		{ -1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
		{  1.0f, -1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
		{  1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },
		{ -1.0f,  1.0f, -1.0f }, {  0.0f,  0.0f, -1.0f },

		{ -1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
		{  1.0f, -1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
		{  1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },
		{ -1.0f,  1.0f,  1.0f }, {  0.0f,  0.0f,  1.0f },

		{ -1.0f, -1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f },
		{ -1.0f,  1.0f, -1.0f }, { -1.0f,  0.0f,  0.0f },
		{ -1.0f,  1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f },
		{ -1.0f, -1.0f,  1.0f }, { -1.0f,  0.0f,  0.0f },

		{  1.0f, -1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f },
		{  1.0f,  1.0f, -1.0f }, {  1.0f,  0.0f,  0.0f },
		{  1.0f,  1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f },
		{  1.0f, -1.0f,  1.0f }, {  1.0f,  0.0f,  0.0f },

		{ -1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f },
		{  1.0f, -1.0f, -1.0f }, {  0.0f, -1.0f,  0.0f },
		{  1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f },
		{ -1.0f, -1.0f,  1.0f }, {  0.0f, -1.0f,  0.0f },

		{ -1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f },
		{  1.0f,  1.0f, -1.0f }, {  0.0f,  1.0f,  0.0f },
		{  1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f },
		{ -1.0f,  1.0f,  1.0f }, {  0.0f,  1.0f,  0.0f }
	};

	uint32_t const GPUBoxMeshGenerator::triangles[36] =
	{
		0, 2, 1,
		0, 3, 2,

		4, 5, 6,
		4, 6, 7,

		8, 10,  9,
		8, 11, 10,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 22, 21,
		20, 23, 22
	};

	GPUMeshGenerationRequirement GPUBoxMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertices_count = (sizeof(vertices) / sizeof(vertices[0])) / 2; //  div by 2 because buffer contains POSITION + NORMAL
		result.indices_count = (sizeof(triangles) / sizeof(triangles[0]));
		return result;
	}

    GPUVertexDeclaration * GPUBoxMeshGenerator::GenerateVertexDeclaration() const
	{
        GPUVertexDeclaration* result = new GPUVertexDeclaration();
        if (result != nullptr)
        {
            result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
            result->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);
        }
        return result;
	}

	void GPUBoxMeshGenerator::GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		GPUMeshElement& mesh_element = elem_create_func();

		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = sizeof(triangles) / sizeof(triangles[0]); // number of triangles does not depends on NORMAL presence
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		mesh_element.primitives.push_back(draw_primitive);

		// the indices
		indices_writer.Write(triangles, sizeof(triangles));

		// the vertices
		glm::vec3 const & hs = primitive.half_size;
		glm::vec3 const & p  = primitive.position;

		box3 bounding_box;

		int const count = sizeof(vertices) / sizeof(vertices[0]);
		for (int i = 0; i < count / 2; ++i)
		{
			glm::vec3 position = vertices[i * 2] * hs + p;
			glm::vec3 normal   = vertices[i * 2 + 1];

			glm::vec3 transformed_position = GLMTools::MultWithTranslation(transform, position);
			glm::vec3 transformed_normal   = GLMTools::Mult(transform, normal);

			vertices_writer << transformed_position;
			vertices_writer << transformed_normal;

			ExtendBox(bounding_box, transformed_position);
		}
	}

	// =====================================================================
	// GPUWireframeBoxMeshGenerator
	// =====================================================================

	glm::vec3 const GPUWireframeBoxMeshGenerator::vertices[8] = // position
	{
		{-1.0f, -1.0f, -1.0f},
		{-1.0f,  1.0f, -1.0f},
		{ 1.0f,  1.0f, -1.0f},
		{ 1.0f, -1.0f, -1.0f},

		{-1.0f, -1.0f,  1.0f},
		{-1.0f,  1.0f,  1.0f},
		{ 1.0f,  1.0f,  1.0f},
		{ 1.0f, -1.0f,  1.0f}
	};

	uint32_t const GPUWireframeBoxMeshGenerator::indices[24] =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		4, 5,
		5, 6,
		6, 7,
		7, 4,

		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	GPUMeshGenerationRequirement GPUWireframeBoxMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertices_count = (sizeof(vertices) / sizeof(vertices[0]));
		result.indices_count = (sizeof(indices) / sizeof(indices[0]));
		return result;
	}

	GPUVertexDeclaration* GPUWireframeBoxMeshGenerator::GenerateVertexDeclaration() const
	{
		GPUVertexDeclaration* result = new GPUVertexDeclaration();
		if (result != nullptr)
		{
			result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
		}
		return result;
	}

	void GPUWireframeBoxMeshGenerator::GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter& vertices_writer, MemoryBufferWriter& indices_writer) const
	{
		GPUMeshElement& mesh_element = elem_create_func();

		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = sizeof(indices) / sizeof(indices[0]);
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_LINES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		mesh_element.primitives.push_back(draw_primitive);

		// the indices
		indices_writer.Write(indices, sizeof(indices));

		// the vertices
		glm::vec3 const & hs = primitive.half_size;
		glm::vec3 const & p  = primitive.position;

		box3 bounding_box;

		size_t const count = sizeof(vertices) / sizeof(vertices[0]);
		for (size_t i = 0; i < count; ++i)
		{
			glm::vec3 position = vertices[i] * hs + p;
			glm::vec3 transformed_position = GLMTools::MultWithTranslation(transform, position);

			vertices_writer << transformed_position;

			ExtendBox(bounding_box, transformed_position);
		}
	}

	// =====================================================================
	// GPUCircleMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUCircleMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertices_count = 1 + subdivisions;
		result.indices_count  = 3 * subdivisions;
		return result;
	}

    GPUVertexDeclaration * GPUCircleMeshGenerator::GenerateVertexDeclaration() const
	{
        GPUVertexDeclaration* result = new GPUVertexDeclaration();
        if (result != nullptr)
        {
            result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
            result->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);
        }
        return result;
	}

	void GPUCircleMeshGenerator::GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		glm::vec3 normal = {0.0f, 0.0f, 1.0f};
		glm::vec3 transformed_normal = GLMTools::Mult(transform, normal);

		// generate the vertices
		box3 bounding_box;

		glm::vec3 center = {primitive.position, 0.0f};
		glm::vec3 transformed_center = GLMTools::MultWithTranslation(transform, center);

		vertices_writer << transformed_center;
		vertices_writer << transformed_normal;

		ExtendBox(bounding_box, transformed_center);

		float delta_alpha = ((float)M_PI * 2.0f) / ((float)subdivisions);
		for (int i = 0 ; i < subdivisions ; ++i)
		{
			float alpha = (float)i * delta_alpha;

			glm::vec2 direction = {std::cos(alpha), std::sin(alpha)};
			glm::vec3 position  = {primitive.radius * direction + primitive.position, 0.0f};

			glm::vec3 transformed_position = GLMTools::MultWithTranslation(transform, position);

			vertices_writer << transformed_position;
			vertices_writer << transformed_normal;

			ExtendBox(bounding_box, transformed_position);
		}

		// generate the index buffer
		for (int i = 0 ; i < subdivisions ; ++i)
		{
			indices_writer << (std::uint32_t)0;
			indices_writer << (std::uint32_t)(i + 1);
			indices_writer << (std::uint32_t)(((i + 1) % subdivisions) + 1);
		}

		// insert the primitive
		int indices_count  = 3 * subdivisions;

		GPUMeshElement& mesh_element = elem_create_func();

		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = indices_count;
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		mesh_element.primitives.push_back(draw_primitive);
	}

	// =====================================================================
	// GPUSphereMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUSphereMeshGenerator::GetRequirement() const
	{
		int subdiv_beta = std::max(subdivisions, 3);
		int subdiv_alpha = subdiv_beta * 2;

		GPUMeshGenerationRequirement result;
		result.vertices_count = 2 + subdiv_beta * subdiv_alpha;
		result.indices_count =
			subdiv_alpha * 3 +
			subdiv_alpha * (subdiv_beta - 1) * 6 +
			subdiv_alpha * 3;
		return result;
	}

    GPUVertexDeclaration * GPUSphereMeshGenerator::GenerateVertexDeclaration() const
	{
        GPUVertexDeclaration* result = new GPUVertexDeclaration();
        if (result != nullptr)
        {
            result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
            result->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);
        }
        return result;
	}

	void GPUSphereMeshGenerator::GenerateMeshData(GPUMeshGenerationElementCreationFunc elem_create_func, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		box3 bounding_box;

		auto InsertVertex = [this, &vertices_writer, &bounding_box](float alpha, float beta)
		{
			glm::vec3 normal = MathTools::PolarCoordToVector(alpha, beta);
			glm::vec3 position = primitive.radius * normal + primitive.position;

			glm::vec3 transformed_position = GLMTools::MultWithTranslation(transform, position);
			glm::vec3 transformed_normal   = GLMTools::Mult(transform, normal);
			
			vertices_writer << transformed_position;
			vertices_writer << transformed_normal;

			ExtendBox(bounding_box, transformed_position);
		};

		int subdiv_beta = std::max(subdivisions, 3);
		int subdiv_alpha = subdiv_beta * 2;

		glm::vec3 position = primitive.position;
		float     radius = primitive.radius;

		// construct the vertex buffer
		InsertVertex(0.0f, (float)M_PI_2);

		float delta_alpha = ((float)M_PI * 2.0f) / ((float)subdiv_alpha); // there is twice more divisions along ALPHA than BETA
		float delta_beta = ((float)M_PI) / ((float)subdiv_beta);

		float beta = (float)M_PI_2 + delta_beta * 0.5f;
		for (int i = 0; i < subdiv_beta; ++i)
		{
			float alpha = 0.0f;
			for (int j = 0; j < subdiv_alpha; ++j)
			{
				InsertVertex(alpha, beta);
				alpha += delta_alpha;
			}
			beta += delta_beta;
		}

		InsertVertex(0.0f, (float)-M_PI_2);

		// construct the index buffer
		for (int i = 0; i < subdiv_alpha; ++i)   // the TOP-CAP
		{
			indices_writer << (std::uint32_t)(1 + i);
			indices_writer << (std::uint32_t)(0);
			indices_writer << (std::uint32_t)(1 + ((i + 1) % subdiv_alpha));
		}

		for (int i = 0; i < subdiv_beta - 1; ++i) // the middle part
		{
			int start_line = 1 + i * subdiv_alpha;
			int next_line = start_line + subdiv_alpha;
			for (int j = 0; j < subdiv_alpha; ++j)
			{
				int next_on_line = ((j + 1) % subdiv_alpha);

				std::uint32_t a = (std::uint32_t)(start_line + j);
				std::uint32_t b = (std::uint32_t)(next_line + j);
				std::uint32_t c = (std::uint32_t)(next_line + next_on_line);
				std::uint32_t d = (std::uint32_t)(start_line + next_on_line);

				indices_writer << b << a << c;
				indices_writer << c << a << d;
			}
		}

		int vertex_count = 2 + subdiv_beta * subdiv_alpha;
		int start_line = 1 + (subdiv_beta - 1) * subdiv_alpha;
		int last_vertex = vertex_count - 1;
		for (int i = 0; i < subdiv_alpha; ++i) // the BOTTOM-CAP
		{
			indices_writer << (std::uint32_t)(last_vertex);
			indices_writer << (std::uint32_t)(start_line + i);
			indices_writer << (std::uint32_t)(start_line + ((i + 1) % subdiv_alpha));
		}

		// the triangles
		int indices_count =
			subdiv_alpha * 3 +
			subdiv_alpha * (subdiv_beta - 1) * 6 +
			subdiv_alpha * 3;

		GPUMeshElement& mesh_element = elem_create_func();

		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = indices_count;
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		mesh_element.primitives.push_back(draw_primitive);
	}

}; // namespace chaos
