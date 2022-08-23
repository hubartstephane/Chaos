#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

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

	glm::vec2 const GPUQuadMeshGenerator::vertices[4] =
	{
		glm::vec2(-1.0f, -1.0f),
		glm::vec2(1.0f, -1.0f),
		glm::vec2(1.0f,  1.0f),
		glm::vec2(-1.0f,  1.0f)
	};

	GLuint const GPUQuadMeshGenerator::triangles[6] =
	{
		0, 1, 2,
		0, 2, 3
	};

	glm::vec3 const GPUCubeMeshGenerator::vertices[24 * 2] = // position + normal
	{
		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(1.0f,  1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),

		glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),
		glm::vec3(1.0f, -1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),
		glm::vec3(1.0f,  1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),
		glm::vec3(-1.0f,  1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),

		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f,  1.0f,  1.0f), glm::vec3(-1.0f, 0.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(-1.0f, 0.0f, 0.0f),

		glm::vec3(+1.0f, -1.0f, -1.0f), glm::vec3(+1.0f, 0.0f, 0.0f),
		glm::vec3(+1.0f,  1.0f, -1.0f), glm::vec3(+1.0f, 0.0f, 0.0f),
		glm::vec3(+1.0f,  1.0f,  1.0f), glm::vec3(+1.0f, 0.0f, 0.0f),
		glm::vec3(+1.0f, -1.0f,  1.0f), glm::vec3(+1.0f, 0.0f, 0.0f),

		glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f),
		glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f),

		glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
		glm::vec3(1.0f, +1.0f, -1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
		glm::vec3(1.0f, +1.0f,  1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
		glm::vec3(-1.0f, +1.0f,  1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
	};

	GLuint const GPUCubeMeshGenerator::triangles[36] =
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

	// =====================================================================
	// GPUMeshGenerationRequirement
	// =====================================================================

	bool GPUMeshGenerationRequirement::IsValid() const
	{
		if (vertex_size <= 0)
			return false;
		if (vertices_count <= 0) // at least one vertex
			return false;
		if (indices_count < 0)
			return false;
		return true;
	}

	// =====================================================================
	// GPUMeshGenerator
	// =====================================================================

	bool GPUMeshGenerator::FillMeshData(GPUMesh* mesh) const
	{
		assert(mesh != nullptr);

		GPUMeshGenerationRequirement requirement = GetRequirement();
		if (!requirement.IsValid())
			return false;

		// create a vertex buffer
		int vb_size = 0;

		shared_ptr<GPUBuffer> vertex_buffer;
		if (requirement.vertices_count > 0)
		{
			vertex_buffer = new GPUBuffer(false);
			if (vertex_buffer == nullptr || !vertex_buffer->IsValid())
				return false;

			vb_size = requirement.vertices_count * requirement.vertex_size;
			vertex_buffer->SetBufferData(nullptr, vb_size);
		}

		// create a index buffer
		int ib_size = 0;

		shared_ptr<GPUBuffer> index_buffer;
		if (requirement.indices_count > 0)
		{
			index_buffer = new GPUBuffer(false);
			if (index_buffer == nullptr || !index_buffer->IsValid())
				return false;

			ib_size = requirement.indices_count * sizeof(GLuint);
			index_buffer->SetBufferData(nullptr, ib_size);
		}

		// map the buffers		
		char* vb_ptr = (vertex_buffer != nullptr) ? vertex_buffer->MapBuffer(0, 0, false, true) : nullptr;
		char* ib_ptr = (index_buffer != nullptr) ? index_buffer->MapBuffer(0, 0, false, true) : nullptr;

		// prepare the mesh
		mesh->Clear(nullptr);

		GPUMeshElement& element = mesh->AddMeshElement(vertex_buffer.get(), index_buffer.get());

		// generate the indices and the vertices
		MemoryBufferWriter vertices_writer(vb_ptr, vb_size);
		MemoryBufferWriter indices_writer(ib_ptr, ib_size);
		GenerateMeshData(element.primitives, vertices_writer, indices_writer);

		assert(vertices_writer.GetRemainingBufferSize() == 0);
		assert(indices_writer.GetRemainingBufferSize() == 0);

		// get the vertex declaration
		element.vertex_declaration = GenerateVertexDeclaration();
		assert(element.vertex_declaration->GetVertexSize() == requirement.vertex_size);

		// initialize the vertex array and validate
		element.vertex_buffer_offset = 0;

		// unmap buffers
		if (vertex_buffer != nullptr)
			vertex_buffer->UnMapBuffer();
		if (index_buffer != nullptr)
			index_buffer->UnMapBuffer();

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
		result.vertex_size = 2 * sizeof(glm::vec3);
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

	void GPUTriangleMeshGenerator::GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = 3;
		draw_primitive.indexed = false;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		primitives.push_back(draw_primitive);

		glm::vec3 normal = glm::normalize(glm::cross(primitive.b - primitive.a, primitive.c - primitive.b));
		normal = GLMTools::Mult(transform, normal);

		vertices_writer << GLMTools::MultWithTranslation(transform, primitive.a);
		vertices_writer << normal;
		vertices_writer << GLMTools::MultWithTranslation(transform, primitive.b);
		vertices_writer << normal;
		vertices_writer << GLMTools::MultWithTranslation(transform, primitive.c);
		vertices_writer << normal;
	}

	// =====================================================================
	// GPUQuadMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUQuadMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertex_size = 2 * sizeof(glm::vec3);
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

	void GPUQuadMeshGenerator::GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = sizeof(triangles) / sizeof(triangles[0]);
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		primitives.push_back(draw_primitive);

		// the indices
		indices_writer.Write(triangles, sizeof(triangles));

		// the vertices 
		int const vertex_count = sizeof(vertices) / sizeof(vertices[0]);

		glm::vec3 normal = GLMTools::Mult(transform, glm::vec3(0.0f, 0.0f, 1.0f));
		for (int i = 0; i < vertex_count; ++i)
		{
			vertices_writer << GLMTools::MultWithTranslation(transform, glm::vec3(vertices[i] * primitive.half_size + primitive.position, 0.0f));
			vertices_writer << normal;
		}
	}

	// =====================================================================
	// GPUCubeMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUCubeMeshGenerator::GetRequirement() const
	{
		GPUMeshGenerationRequirement result;
		result.vertex_size = 2 * sizeof(glm::vec3);
		result.vertices_count = (sizeof(vertices) / sizeof(vertices[0])) / 2; //  div by 2 because buffer contains POSITION + NORMAL
		result.indices_count = (sizeof(triangles) / sizeof(triangles[0]));
		return result;
	}

    GPUVertexDeclaration * GPUCubeMeshGenerator::GenerateVertexDeclaration() const
	{
        GPUVertexDeclaration* result = new GPUVertexDeclaration();
        if (result != nullptr)
        {
            result->Push(VertexAttributeSemantic::POSITION, 0, VertexAttributeType::FLOAT3);
            result->Push(VertexAttributeSemantic::NORMAL, 0, VertexAttributeType::FLOAT3);
        }
        return result;
	}

	void GPUCubeMeshGenerator::GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		// the primitives
		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = sizeof(triangles) / sizeof(triangles[0]); // number of triangles does not depends on NORMAL presence
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		primitives.push_back(draw_primitive);

		// the indices
		indices_writer.Write(triangles, sizeof(triangles));

		// the vertices
		glm::vec3 hs = primitive.half_size;
		glm::vec3 p = primitive.position;

		int const count = sizeof(vertices) / sizeof(vertices[0]);
		for (int i = 0; i < count / 2; ++i)
		{
			vertices_writer << GLMTools::MultWithTranslation(transform, vertices[i * 2] * hs + p); // resize position
			vertices_writer << GLMTools::Mult(transform, vertices[i * 2 + 1]);      // copy normal
		}
	}

	// =====================================================================
	// GPUCircleMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUCircleMeshGenerator::GetRequirement() const 
	{
		GPUMeshGenerationRequirement result;
		result.vertex_size = 2 * sizeof(glm::vec3);
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

	void GPUCircleMeshGenerator::GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const 
	{
		glm::vec3 normal = GLMTools::Mult(transform, glm::vec3(0.0f, 0.0f, 1.0f));
	
		// generate the vertices
		vertices_writer << GLMTools::MultWithTranslation(transform, glm::vec3(primitive.position, 0.0f));
		vertices_writer << normal;

		float delta_alpha = ((float)M_PI * 2.0f) / ((float)subdivisions); 
		for (int i = 0 ; i < subdivisions ; ++i)
		{
			float alpha = (float)i * delta_alpha;

			glm::vec2 direction = glm::vec2(std::cos(alpha), std::sin(alpha));

			vertices_writer << GLMTools::MultWithTranslation(transform, glm::vec3(primitive.radius * direction + primitive.position, 0.0f));
			vertices_writer << normal;
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

		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = indices_count;
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		primitives.push_back(draw_primitive);
	}

	// =====================================================================
	// GPUSphereMeshGenerator
	// =====================================================================

	GPUMeshGenerationRequirement GPUSphereMeshGenerator::GetRequirement() const
	{
		int subdiv_beta = std::max(subdivisions, 3);
		int subdiv_alpha = subdiv_beta * 2;

		GPUMeshGenerationRequirement result;
		result.vertex_size = 2 * sizeof(glm::vec3);
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

	void GPUSphereMeshGenerator::GenerateMeshData(std::vector<GPUDrawPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
	{
		int subdiv_beta = std::max(subdivisions, 3);
		int subdiv_alpha = subdiv_beta * 2;

		glm::vec3 position = primitive.position;
		float     radius = primitive.radius;

		// construct the vertex buffer
		InsertVertex(vertices_writer, 0.0f, (float)M_PI_2);

		float delta_alpha = ((float)M_PI * 2.0f) / ((float)subdiv_alpha); // there is twice more divisions along ALPHA than BETA
		float delta_beta = ((float)M_PI) / ((float)subdiv_beta);

		float beta = (float)M_PI_2 + delta_beta * 0.5f;
		for (int i = 0; i < subdiv_beta; ++i)
		{
			float alpha = 0.0f;
			for (int j = 0; j < subdiv_alpha; ++j)
			{
				InsertVertex(vertices_writer, alpha, beta);
				alpha += delta_alpha;
			}
			beta += delta_beta;
		}

		InsertVertex(vertices_writer, 0.0f, (float)-M_PI_2);

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

		GPUDrawPrimitive draw_primitive;
		draw_primitive.count = indices_count;
		draw_primitive.indexed = true;
		draw_primitive.primitive_type = GL_TRIANGLES;
		draw_primitive.start = 0;
		draw_primitive.base_vertex_index = 0;
		primitives.push_back(draw_primitive);
	}

	void GPUSphereMeshGenerator::InsertVertex(MemoryBufferWriter & vertices_writer, float alpha, float beta) const
	{
		glm::vec3 normal = MathTools::PolarCoordToVector(alpha, beta);
		vertices_writer << GLMTools::MultWithTranslation(transform, primitive.radius * normal + primitive.position);
		vertices_writer << GLMTools::Mult(transform, normal);
	}

}; // namespace chaos
