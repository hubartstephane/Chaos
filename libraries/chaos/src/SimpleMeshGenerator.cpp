#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLTools.h>
#include <chaos/MathTools.h>


#include <boost/iostreams/stream_buffer.hpp>

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

glm::vec3 const QuadMeshGeneratorProxy::vertices[4] =
{
  glm::vec3(-1.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f,  1.0f, 0.0f),
  glm::vec3(-1.0f,  1.0f, 0.0f)
};

GLuint const QuadMeshGeneratorProxy::triangles[6] =
{
  0, 1, 2,  
  0, 2, 3
};

glm::vec3 const CubeMeshGeneratorProxy::vertices[24 * 2] = // position + normal
{
  glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3( 1.0f,  1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),
  glm::vec3(-1.0f,  1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f),

  glm::vec3(-1.0f, -1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),
  glm::vec3( 1.0f, -1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),
  glm::vec3( 1.0f,  1.0f, +1.0f), glm::vec3(0.0f, 0.0f, +1.0f),
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
  glm::vec3( 1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f),
  glm::vec3(-1.0f, -1.0f,  1.0f), glm::vec3(0.0f, -1.0f, 0.0f),

  glm::vec3(-1.0f, +1.0f, -1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
  glm::vec3( 1.0f, +1.0f, -1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
  glm::vec3( 1.0f, +1.0f,  1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
  glm::vec3(-1.0f, +1.0f,  1.0f), glm::vec3(0.0f, +1.0f, 0.0f),
};

GLuint const CubeMeshGeneratorProxy::triangles[36] =
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


SimpleMeshGeneratorProxy * QuadMeshGenerator::CreateProxy() const { return new QuadMeshGeneratorProxy(*this); }

SimpleMeshGeneratorProxy * CubeMeshGenerator::CreateProxy() const { return new CubeMeshGeneratorProxy(*this); }

SimpleMeshGeneratorProxy * SphereMeshGenerator::CreateProxy() const { return new SphereMeshGeneratorProxy(*this); }

bool MeshGenerationRequirement::IsValid() const
{
  if (vertices_count <= 0) // at least one vertex
    return false;
  if (indices_count < 0)
    return false;
  return true;
}

boost::intrusive_ptr<SimpleMesh> SimpleMeshGenerator::GenerateMesh() const
{
  boost::intrusive_ptr<SimpleMesh> result;

  SimpleMeshGeneratorProxy * proxy = CreateProxy();
  if (proxy != nullptr)
  {
    MeshGenerationRequirement requirement = proxy->GetRequirement();
    if (requirement.IsValid())
    {
      boost::intrusive_ptr<SimpleMesh> mesh = new SimpleMesh();
      if (mesh != nullptr)
      {
        boost::intrusive_ptr<VertexBuffer> * vb_ptr = (requirement.vertices_count > 0) ? &mesh->vertex_buffer : nullptr;
        boost::intrusive_ptr<IndexBuffer>  * ib_ptr = (requirement.indices_count  > 0) ? &mesh->index_buffer : nullptr;

        if (GLTools::GenerateVertexAndIndexBuffersObject(&mesh->vertex_array, vb_ptr, ib_ptr))
        {
          // get the vertex declaration
          proxy->GenerateVertexDeclaration(mesh->declaration);

          // allocate buffer for vertices and indices
          char   * vertices = nullptr;
          GLuint * indices  = nullptr;

          size_t vertex_size = mesh->declaration.GetVertexSize();
          size_t vb_size     = requirement.vertices_count * vertex_size;

          if (requirement.vertices_count > 0)
            vertices = new char[vb_size];
          if (requirement.indices_count > 0)
            indices = new GLuint[requirement.indices_count];

          // generate the indices and the vertices
          MemoryBufferWriter vertices_writer(vertices, vb_size);
          MemoryBufferWriter indices_writer(indices, requirement.indices_count * sizeof(GLuint));
          proxy->GenerateMeshData(mesh->primitives, vertices_writer, indices_writer);

          size_t remaining_vertices = vertices_writer.GetRemainingBufferSize();
          size_t remaining_indices  = indices_writer.GetRemainingBufferSize();

          // transfert data top GPU and free memory
          if (vertices != nullptr)
          {
            glNamedBufferData(mesh->vertex_buffer->GetResourceID(), vb_size, vertices, GL_STATIC_DRAW);
            delete[] vertices;
          }
          if (indices != nullptr)
          {
            glNamedBufferData(mesh->index_buffer->GetResourceID(), requirement.indices_count * sizeof(GLuint), indices, GL_STATIC_DRAW);
            delete[] indices;
          }

          // initialize the vertex array
          mesh->FinalizeBindings();

          result = mesh;
        }
      }
    }
    delete proxy;
  }
  return result;
}

MeshGenerationRequirement QuadMeshGeneratorProxy::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertices_count = sizeof(vertices)  / sizeof(vertices[0]);
  result.indices_count  = sizeof(triangles) / sizeof(triangles[0]);
  return result;
}

void QuadMeshGeneratorProxy::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
}

void QuadMeshGeneratorProxy::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
{
  // the primitives
  MeshPrimitive mesh_primitive;
  mesh_primitive.count = sizeof(triangles) / sizeof(triangles[0]);
  mesh_primitive.indexed = true;
  mesh_primitive.primitive_type = GL_TRIANGLES;
  mesh_primitive.start = 0;
  mesh_primitive.base_vertex_index = 0;
  primitives.push_back(mesh_primitive);

  // the indices
  indices_writer.Write(triangles, sizeof(triangles));

  // the vertices 
  glm::vec3 hs = glm::vec3(generator.primitive.half_size.x, generator.primitive.half_size.y, 1.0f);
  glm::vec3 p  = glm::vec3(generator.primitive.position.x, generator.primitive.position.y,  0.0f);

  int const vertex_count = sizeof(vertices) / sizeof(vertices[0]); 
  
  for (int i = 0; i < vertex_count; ++i)
    vertices_writer << (vertices[i] * hs + p);
}

MeshGenerationRequirement CubeMeshGeneratorProxy::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertices_count = (sizeof(vertices) / sizeof(vertices[0])) / 2; //  div by 2 because buffer contains POSITION + NORMAL
  result.indices_count  = (sizeof(triangles) / sizeof(triangles[0]));
  return result;
}

void CubeMeshGeneratorProxy::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
  declaration.Push(chaos::SEMANTIC_NORMAL, 0, chaos::TYPE_FLOAT3);
}

void CubeMeshGeneratorProxy::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
{
  // the primitives
  MeshPrimitive mesh_primitive;
  mesh_primitive.count             = sizeof(triangles) / sizeof(triangles[0]); // number of triangles does not depends on NORMAL presence
  mesh_primitive.indexed           = true;
  mesh_primitive.primitive_type    = GL_TRIANGLES;
  mesh_primitive.start             = 0;
  mesh_primitive.base_vertex_index = 0;
  primitives.push_back(mesh_primitive);

  // the indices
  indices_writer.Write(triangles, sizeof(triangles));

  // the vertices
  glm::vec3 hs = generator.primitive.half_size;
  glm::vec3 p  = generator.primitive.position;  

  int const count = sizeof(vertices) / sizeof(vertices[0]);
  for (int i = 0; i < count / 2; ++i)
  {
    vertices_writer << vertices[i * 2] * hs + p; // resize position
    vertices_writer << vertices[i * 2 + 1];      // copy normal
  }
}

MeshGenerationRequirement SphereMeshGeneratorProxy::GetRequirement() const
{
  int subdiv_beta  = max(generator.subdivisions, 3);
  int subdiv_alpha = subdiv_beta * 2;

  MeshGenerationRequirement result;
  result.vertices_count = 2 + subdiv_beta * subdiv_alpha;
  result.indices_count =
    subdiv_alpha * 3 +
    subdiv_alpha * (subdiv_beta - 1) * 6 +
    subdiv_alpha * 3;
  return result;
}

void SphereMeshGeneratorProxy::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
}

void SphereMeshGeneratorProxy::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
{
  int subdiv_beta  = max(generator.subdivisions, 3);
  int subdiv_alpha = subdiv_beta * 2;

  // construct the vertex buffer
  vertices_writer << GetSphereVertex(0.0f, (float)M_PI_2);

  float delta_alpha = ((float)M_PI * 2.0f) / ((float)subdiv_alpha); // there is twice more divisions along ALPHA than BETA
  float delta_beta = ((float)M_PI) / ((float)subdiv_beta);

  float beta = (float)M_PI_2 + delta_beta * 0.5f;
  for (int i = 0; i < subdiv_beta; ++i)
  {
    float alpha = 0.0f;
    for (int j = 0; j < subdiv_alpha; ++j)
    {
      vertices_writer << GetSphereVertex(alpha, beta);
      alpha += delta_alpha;
    }
    beta += delta_beta;
  }

  vertices_writer << GetSphereVertex(0.0f, (float)-M_PI_2);

  // construct the index buffer
  for (int i = 0; i < subdiv_alpha; ++i)   // the TOP-CAP
  {
    indices_writer << (GLuint)(1 + i);
    indices_writer << (GLuint)0;
    indices_writer << (GLuint)(1 + ((i + 1) % subdiv_alpha));
  }

  for (int i = 0; i < subdiv_beta - 1; ++i) // the middle part
  {
    int start_line = 1 + i * subdiv_alpha;
    int next_line = start_line + subdiv_alpha;
    for (int j = 0; j < subdiv_alpha; ++j)
    {
      GLint next_on_line = ((j + 1) % subdiv_alpha);

      GLint a = start_line + j;
      GLint b = next_line + j;
      GLint c = next_line + next_on_line;
      GLint d = start_line + next_on_line;

      indices_writer << b << a << c;
      indices_writer << c << a << d;
    }
  }

  int vertex_count = 2 + subdiv_beta * subdiv_alpha;
  int start_line   = 1 + (subdiv_beta - 1) * subdiv_alpha;
  int last_vertex  = vertex_count - 1;
  for (int i = 0; i < subdiv_alpha; ++i) // the BOTTOM-CAP
  {
    indices_writer << (GLuint)(last_vertex);
    indices_writer << (GLuint)(start_line + i);
    indices_writer << (GLuint)(start_line + ((i + 1) % subdiv_alpha));
  }

  // the triangles
  int indices_count =
    subdiv_alpha * 3 +
    subdiv_alpha * (subdiv_beta - 1) * 6 +
    subdiv_alpha * 3;

  MeshPrimitive mesh_primitive;
  mesh_primitive.count             = indices_count;
  mesh_primitive.indexed           = true;
  mesh_primitive.primitive_type    = GL_TRIANGLES;
  mesh_primitive.start             = 0;
  mesh_primitive.base_vertex_index = 0;
  primitives.push_back(mesh_primitive);
}

float3 SphereMeshGeneratorProxy::GetSphereVertex(float alpha, float beta) const
{
  return generator.primitive.radius * MathTools::PolarCoordToVector(alpha, beta) + generator.primitive.position;
}

}; // namespace chaos
