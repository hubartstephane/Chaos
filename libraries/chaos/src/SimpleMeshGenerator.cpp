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

glm::vec3 const CubeMeshGeneratorProxy::vertices[8] =
{
  glm::vec3(-1.0f, -1.0f, -1.0f),
  glm::vec3( 1.0f, -1.0f, -1.0f),
  glm::vec3( 1.0f,  1.0f, -1.0f),
  glm::vec3(-1.0f,  1.0f, -1.0f),
  glm::vec3(-1.0f, -1.0f,  1.0f),
  glm::vec3( 1.0f, -1.0f,  1.0f),
  glm::vec3( 1.0f,  1.0f,  1.0f),
  glm::vec3(-1.0f,  1.0f,  1.0f)
};

GLuint const CubeMeshGeneratorProxy::triangles[36] =
{
  0, 1, 5,
  0, 5, 4,
  3, 2, 1,
  3, 1, 0,
  2, 6, 5,
  2, 5, 1,
  7, 6, 2,
  7, 2, 3,
  6, 7, 4,
  6, 4, 5,
  7, 3, 0,
  7, 0, 4
};

glm::vec3 const CubeMeshGeneratorProxy::vertices_with_normals[24 * 2] =
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

GLuint const CubeMeshGeneratorProxy::triangles_with_normals[36] =
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
  SimpleMesh * result = nullptr;

  SimpleMeshGeneratorProxy * proxy = CreateProxy();
  if (proxy != nullptr)
  {
    MeshGenerationRequirement requirement = proxy->GetRequirement();
    if (requirement.IsValid())
    {
      SimpleMesh * mesh = new SimpleMesh();
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
          MemoryBufferWriter vertices_buffer(vertices, vb_size);
          MemoryBufferWriter indices_buffer(indices, requirement.indices_count * sizeof(GLuint));
          proxy->GenerateMeshData(mesh->primitives, vertices_buffer, indices_buffer);

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

void QuadMeshGeneratorProxy::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_buffer, MemoryBufferWriter & indices_buffer) const
{
  MeshPrimitive mesh_primitive;
  mesh_primitive.count = sizeof(triangles) / sizeof(triangles[0]);
  mesh_primitive.indexed = true;
  mesh_primitive.primitive_type = GL_TRIANGLES;
  mesh_primitive.start = 0;
  mesh_primitive.base_vertex_index = 0;
  primitives.push_back(mesh_primitive);








}



#if 0

boost::intrusive_ptr<SimpleMesh> QuadMeshGenerator::GenerateMesh() const
{
  boost::intrusive_ptr<SimpleMesh> result = new SimpleMesh();
  if (result != nullptr)
  {
    if (GLTools::GenerateVertexAndIndexBuffersObject(&result->vertex_array, &result->vertex_buffer, &result->index_buffer))
    {
      result->declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);

      MeshPrimitive mesh_primitive;
      mesh_primitive.count             = sizeof(triangles) / sizeof(triangles[0]);
      mesh_primitive.indexed           = true;
      mesh_primitive.primitive_type    = GL_TRIANGLES;
      mesh_primitive.start             = 0;
      mesh_primitive.base_vertex_index = 0;
      result->primitives.push_back(mesh_primitive);

      // fill the buffers
      glm::vec3 hs = glm::vec3(primitive.half_size.x, primitive.half_size.y, 1.0f);
      glm::vec3 p  = glm::vec3(primitive.position.x, primitive.position.y,  0.0f);

      int const count = sizeof(vertices) / sizeof(vertices[0]); 

      glm::vec3 final_vertices[count]; 
      for (int i = 0; i < count; ++i)
        final_vertices[i] = vertices[i] * hs + p;

      glNamedBufferData(result->vertex_buffer->GetResourceID(), sizeof(glm::vec3) * count, final_vertices, GL_STATIC_DRAW);
      glNamedBufferData(result->index_buffer->GetResourceID(), sizeof(triangles), triangles, GL_STATIC_DRAW);

      // initialize the vertex array
      result->FinalizeBindings();

      return result;
    }
  }
  return nullptr;
}

#endif







MeshGenerationRequirement CubeMeshGeneratorProxy::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertices_count = sizeof(vertices) / sizeof(vertices[0]);
  result.indices_count  = sizeof(triangles) / sizeof(triangles[0]);
  return result;

}

void CubeMeshGeneratorProxy::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
  if (generator.with_normals)
    declaration.Push(chaos::SEMANTIC_NORMAL, 0, chaos::TYPE_FLOAT3);

}

void CubeMeshGeneratorProxy::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_buffer, MemoryBufferWriter & indices_buffer) const
{
  MeshPrimitive mesh_primitive;
  mesh_primitive.count             = sizeof(triangles) / sizeof(triangles[0]); // number of triangles does not depends on NORMAL presence
  mesh_primitive.indexed           = true;
  mesh_primitive.primitive_type    = GL_TRIANGLES;
  mesh_primitive.start             = 0;
  mesh_primitive.base_vertex_index = 0;
  primitives.push_back(mesh_primitive);


}

#if 0

boost::intrusive_ptr<SimpleMesh> CubeMeshGenerator::GenerateMesh() const
{
  boost::intrusive_ptr<SimpleMesh> result = new SimpleMesh();
  if (result != nullptr)
  {
    if (GLTools::GenerateVertexAndIndexBuffersObject(&result->vertex_array, &result->vertex_buffer, &result->index_buffer))
    {
      result->declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
      if (with_normals)
        result->declaration.Push(chaos::SEMANTIC_NORMAL, 0, chaos::TYPE_FLOAT3);

      MeshPrimitive mesh_primitive;
      mesh_primitive.count             = sizeof(triangles) / sizeof(triangles[0]); // number of triangles does not depends on NORMAL presence
      mesh_primitive.indexed           = true;
      mesh_primitive.primitive_type    = GL_TRIANGLES;
      mesh_primitive.start             = 0;
      mesh_primitive.base_vertex_index = 0;
      result->primitives.push_back(mesh_primitive);

      // resize the mesh      
      if (with_normals)
      {
        int const count = sizeof(vertices_with_normals) / sizeof(vertices_with_normals[0]); // number of vertex * number of component

        glm::vec3 final_vertices[count];
        for (int i = 0 ; i < count / 2 ; ++i)
        {        
          final_vertices[i * 2]     = vertices_with_normals[i * 2] * primitive.half_size + primitive.position; // resize position
          final_vertices[i * 2 + 1] = vertices_with_normals[i * 2 + 1];    // copy normal
        }

        glNamedBufferData(result->vertex_buffer->GetResourceID(), sizeof(glm::vec3) * count, final_vertices, GL_STATIC_DRAW);
        glNamedBufferData(result->index_buffer->GetResourceID(), sizeof(triangles_with_normals), triangles_with_normals, GL_STATIC_DRAW);
      }
      else
      {
        int const count = sizeof(vertices) / sizeof(vertices[0]);

        glm::vec3 final_vertices[count];
        for (int i = 0; i < count; ++i)
          final_vertices[i] = vertices[i] * primitive.half_size + primitive.position;

        glNamedBufferData(result->vertex_buffer->GetResourceID(), sizeof(glm::vec3) * count, final_vertices, GL_STATIC_DRAW);
        glNamedBufferData(result->index_buffer->GetResourceID(), sizeof(triangles), triangles, GL_STATIC_DRAW);
      }
         
      // initialize the vertex array
      result->FinalizeBindings(); 

      return result;
    }
  }
  return nullptr;
}
#endif













MeshGenerationRequirement SphereMeshGeneratorProxy::GetRequirement() const
{
  MeshGenerationRequirement result;
  result.vertices_count = 0;
  result.indices_count  = 0;
  return result;
}

void SphereMeshGeneratorProxy::GenerateVertexDeclaration(VertexDeclaration & declaration) const
{
  declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
}

void SphereMeshGeneratorProxy::GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_buffer, MemoryBufferWriter & indices_buffer) const
{


  //typedef boost::iostreams::basic_array_source<char> Device;
  //boost::iostreams::stream<Device> stream(buf, 1024);
  /*
  uint16_t word1, word2;
  stream.read((char*)&word1, sizeof(word1));
  stream.read((char*)&word2, sizeof(word2));
  std::cout << word1 << "," << word2 << std::endl;
  */


}


#if 0

boost::intrusive_ptr<SimpleMesh> SphereMeshGenerator::GenerateMesh() const
{
  boost::intrusive_ptr<SimpleMesh> result = new SimpleMesh();
  if (result != nullptr)
  {
    if (GLTools::GenerateVertexAndIndexBuffersObject(&result->vertex_array, &result->vertex_buffer, &result->index_buffer))
    {
      int subdiv_beta  = max(subdivisions, 3);
      int subdiv_alpha = subdiv_beta * 2;
     
      // construct the vertex declaration
      result->declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);

      std::vector<float3> vertices;
      std::vector<GLuint> indices;

      // construct the vertex buffer
      vertices.push_back(GetSphereVertex(0.0f, (float)M_PI_2));

      float delta_alpha = ((float)M_PI * 2.0f) / ((float)subdiv_alpha); // there is twice more divisions along ALPHA than BETA
      float delta_beta  = ((float)M_PI)       / ((float)subdiv_beta);

      float beta = (float)M_PI_2 + delta_beta * 0.5f;
      for (int i = 0 ; i < subdiv_beta ; ++i)
      {
        float alpha = 0.0f;
        for (int j = 0 ; j < subdiv_alpha ; ++j)
        {
          vertices.push_back(GetSphereVertex(alpha, beta));
          alpha += delta_alpha;
        }
        beta += delta_beta;
      }

      vertices.push_back(GetSphereVertex(0.0f, (float)-M_PI_2));

      // construct the index buffer
      for (int i = 0 ; i < subdiv_alpha ; ++i)
      {        
        indices.push_back(1 + i);
        indices.push_back(0);
        indices.push_back(1 + ((i + 1) % subdiv_alpha));
      }

      for (int i = 0 ; i < subdiv_beta - 1 ; ++i)
      {
        int start_line = 1 + i * subdiv_alpha;
        int next_line  = start_line + subdiv_alpha;
        for (int j = 0 ; j < subdiv_alpha ; ++j)
        {
          GLint next_on_line = ((j + 1) % subdiv_alpha);

          GLint a = start_line + j;
          GLint b = next_line  + j;
          GLint c = next_line  + next_on_line;
          GLint d = start_line + next_on_line;

          indices.push_back(b);
          indices.push_back(a);          
          indices.push_back(c);
          
          indices.push_back(c);
          indices.push_back(a);                    
          indices.push_back(d);
        }
      }

      int start_line  = 1 + (subdiv_beta - 1) * subdiv_alpha;
      int last_vertex = vertices.size() - 1;
      for (int i = 0 ; i < subdiv_alpha ; ++i)
      {
        indices.push_back(last_vertex);
        indices.push_back(start_line + i);
        indices.push_back(start_line + ((i + 1) % subdiv_alpha));
      }

      // the triangles
      MeshPrimitive mesh_primitive;
      mesh_primitive.count             = indices.size();
      mesh_primitive.indexed           = true;
      mesh_primitive.primitive_type    = GL_TRIANGLES;
      mesh_primitive.start             = 0;
      mesh_primitive.base_vertex_index = 0;
      result->primitives.push_back(mesh_primitive);

      // send the buffers to GPU
      glNamedBufferData(result->vertex_buffer->GetResourceID(), sizeof(float3) * vertices.size(), &vertices[0], GL_STATIC_DRAW);
      glNamedBufferData(result->index_buffer->GetResourceID(), sizeof(GLuint) * indices.size(), &indices[0], GL_STATIC_DRAW);

      // initialize the vertex array
      result->FinalizeBindings();

      return result;
    }
  }
  return nullptr;
}

#endif

float3 SphereMeshGeneratorProxy::GetSphereVertex(float alpha, float beta) const
{
  return generator.primitive.radius * MathTools::PolarCoordToVector(alpha, beta) + generator.primitive.position;
}

}; // namespace chaos
