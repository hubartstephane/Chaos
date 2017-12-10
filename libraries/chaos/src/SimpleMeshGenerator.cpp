#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLTools.h>
#include <chaos/MathTools.h>

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

glm::vec3 const QuadMeshGenerator::vertices[4] =
{
  glm::vec3(-1.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f,  1.0f, 0.0f),
  glm::vec3(-1.0f,  1.0f, 0.0f)
};

GLuint const QuadMeshGenerator::triangles[6] =
{
  0, 1, 2,  
  0, 2, 3
};

glm::vec3 const CubeMeshGenerator::vertices[24 * 2] = // position + normal
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

GLuint const CubeMeshGenerator::triangles[36] =
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

bool MeshGenerationRequirement::IsValid() const
{
  if (vertex_size <= 0) 
    return false;
  if (vertices_count <= 0) // at least one vertex
    return false;
  if (indices_count < 0)
    return false;
  return true;
}

boost::intrusive_ptr<SimpleMesh> SimpleMeshGenerator::GenerateMesh() const
{
  boost::intrusive_ptr<SimpleMesh> result;

  MeshGenerationRequirement requirement = GetRequirement();
  if (requirement.IsValid())
  {
    boost::intrusive_ptr<SimpleMesh> mesh = new SimpleMesh();
    if (mesh != nullptr)
    {
      boost::intrusive_ptr<VertexBuffer> * vb_ptr = (requirement.vertices_count > 0) ? &mesh->vertex_buffer : nullptr;
      boost::intrusive_ptr<IndexBuffer>  * ib_ptr = (requirement.indices_count  > 0) ? &mesh->index_buffer : nullptr;

      if (GLTools::GenerateVertexAndIndexBuffersObject(&mesh->vertex_array, vb_ptr, ib_ptr))
      {
        GLuint vb = (requirement.vertices_count > 0) ? mesh->vertex_array->GetResourceID() : 0;
        GLuint ib = (requirement.indices_count  > 0) ? mesh->index_buffer->GetResourceID() : 0;

        int vb_size = requirement.vertices_count * requirement.vertex_size;
        int ib_size = requirement.indices_count * sizeof(GLuint);

        // allocate buffer for vertices and indices
        std::pair<char *, GLuint *> mapping;
        if (GLTools::MapBuffers(vb, ib, vb_size, ib_size, mapping))
        {
          // generate the indices and the vertices
          MemoryBufferWriter vertices_writer(mapping.first, vb_size);
          MemoryBufferWriter indices_writer(mapping.second, ib_size);
          GenerateMeshData(mesh->primitives, vertices_writer, indices_writer);

          assert(vertices_writer.GetRemainingBufferSize() == 0);
          assert(indices_writer.GetRemainingBufferSize() == 0);

          // get the vertex declaration
          GenerateVertexDeclaration(mesh->declaration);
          assert(mesh->declaration.GetVertexSize() == requirement.vertex_size);

          // initialize the vertex array and validate
          mesh->FinalizeBindings();
          result = mesh;

          // transfert data to GPU and free memory
          if (vb != 0)
            glUnmapNamedBuffer(vb);
          if (ib != 0)
            glUnmapNamedBuffer(ib);
        }
      }
    }
  }

  return result;
}

MeshGenerationRequirement TriangleMeshGenerator::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertex_size = sizeof(glm::vec3);
  result.vertices_count = 3;
  result.indices_count = 0;
  return result;
}

void TriangleMeshGenerator::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT3);
}

void TriangleMeshGenerator::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
{
  // the primitives
  MeshPrimitive mesh_primitive;
  mesh_primitive.count = 3;
  mesh_primitive.indexed = false;
  mesh_primitive.primitive_type = GL_TRIANGLES;
  mesh_primitive.start = 0;
  mesh_primitive.base_vertex_index = 0;
  primitives.push_back(mesh_primitive);

  vertices_writer << primitive.a;
  vertices_writer << primitive.b;  
  vertices_writer << primitive.c;
}

MeshGenerationRequirement QuadMeshGenerator::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertex_size    = sizeof(glm::vec3);
  result.vertices_count = sizeof(vertices)  / sizeof(vertices[0]);
  result.indices_count  = sizeof(triangles) / sizeof(triangles[0]);
  return result;
}

void QuadMeshGenerator::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT3);
}

void QuadMeshGenerator::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
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
  glm::vec3 hs = glm::vec3(primitive.half_size.x, primitive.half_size.y, 1.0f);
  glm::vec3 p  = glm::vec3(primitive.position.x, primitive.position.y,  0.0f);

  int const vertex_count = sizeof(vertices) / sizeof(vertices[0]); 
  
  for (int i = 0; i < vertex_count; ++i)
    vertices_writer << (vertices[i] * hs + p);
}

MeshGenerationRequirement CubeMeshGenerator::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertex_size    = 2 * sizeof(glm::vec3);
  result.vertices_count = (sizeof(vertices) / sizeof(vertices[0])) / 2; //  div by 2 because buffer contains POSITION + NORMAL
  result.indices_count  = (sizeof(triangles) / sizeof(triangles[0]));
  return result;
}

void CubeMeshGenerator::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT3);
  declaration.Push(SEMANTIC_NORMAL, 0, TYPE_FLOAT3);
}

void CubeMeshGenerator::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
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
  glm::vec3 hs = primitive.half_size;
  glm::vec3 p  = primitive.position;  

  int const count = sizeof(vertices) / sizeof(vertices[0]);
  for (int i = 0; i < count / 2; ++i)
  {
    vertices_writer << vertices[i * 2] * hs + p; // resize position
    vertices_writer << vertices[i * 2 + 1];      // copy normal
  }
}

MeshGenerationRequirement SphereMeshGenerator::GetRequirement() const
{
  int subdiv_beta  = max(subdivisions, 3);
  int subdiv_alpha = subdiv_beta * 2;

  MeshGenerationRequirement result;
  result.vertex_size    = 2 * sizeof(glm::vec3);
  result.vertices_count = 2 + subdiv_beta * subdiv_alpha;
  result.indices_count  =
    subdiv_alpha * 3 +
    subdiv_alpha * (subdiv_beta - 1) * 6 +
    subdiv_alpha * 3;
  return result;
}

void SphereMeshGenerator::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(SEMANTIC_POSITION, 0, TYPE_FLOAT3);
  declaration.Push(SEMANTIC_NORMAL, 0, TYPE_FLOAT3);
}

void SphereMeshGenerator::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const
{
  int subdiv_beta  = max(subdivisions, 3);
  int subdiv_alpha = subdiv_beta * 2;
  
  glm::vec3 position = primitive.position;
  float     radius   = primitive.radius;

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

void SphereMeshGenerator::InsertVertex(MemoryBufferWriter & vertices_writer, float alpha, float beta) const
{
  glm::vec3 normal = MathTools::PolarCoordToVector(alpha, beta);
  vertices_writer << primitive.radius * normal + primitive.position;
  vertices_writer << normal;
}

}; // namespace chaos
