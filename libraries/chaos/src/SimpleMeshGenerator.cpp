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

glm::vec3 const CubeMeshGenerator::vertices[8] =
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

GLuint const CubeMeshGenerator::triangles[36] =
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

glm::vec3 const CubeMeshGenerator::vertices_with_normals[24 * 2] =
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

GLuint const CubeMeshGenerator::triangles_with_normals[36] =
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


boost::intrusive_ptr<SimpleMesh> SphereMeshGenerator::GenerateMesh() const
{
  boost::intrusive_ptr<SimpleMesh> result = new SimpleMesh();
  if (result != nullptr)
  {
    if (GLTools::GenerateVertexAndIndexBuffersObject(&result->vertex_array, &result->vertex_buffer, &result->index_buffer))
    {
      int subdiv = max(subdivisions, 3);
     
      // construct the vertex declaration
      result->declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);

      std::vector<float3> vertices;
      std::vector<GLuint> indices;

      // construct the vertex buffer
      vertices.push_back(GetSphereVertex(0.0f, (float)M_PI_2));

      float delta_alpha = ((float)M_2_PI) / ((float)subdiv * 2.0f); // there is twice more divisions along ALPHA than BETA
      float delta_beta  = ((float)M_PI)   / ((float)subdiv);

      float beta = (float)M_PI_2 + delta_beta * 0.5f;
      for (int i = 0 ; i < subdiv ; ++i)
      {
        float alpha = 0.0f;
        for (int j = 0 ; j < subdiv * 2 ; ++j)
        {
          vertices.push_back(GetSphereVertex(alpha, beta));
          alpha += delta_alpha;
        }
        beta += delta_beta;
      }

      vertices.push_back(GetSphereVertex(0.0f, (float)-M_PI_2));

      // construct the index buffer

      indices.push_back(0);
      indices.push_back(1);
      indices.push_back(2);

      /*
      for (int i = 0 ; i < subdiv * 2 ; ++i)
      {
        indices.push_back(0);
        indices.push_back(1 + i);
        indices.push_back(1 + ((i + 1) % (subdiv * 2)));
      }
      */
#if 0
      for (int i = 0 ; i < subdiv ; ++i)
      {
        int start_line = 1 + i;
        int next_line  = 1 + i + (subdiv * 2);
        for (int j = 0 ; j < subdiv * 2 ; ++j)
        {
          GLint next_on_line = ((j + 1) % (subdiv * 2));

          GLint a = start_line + j;
          GLint b = next_line  + j;
          GLint c = next_line  + next_on_line;
          GLint d = start_line + next_on_line;

          indices.push_back(a);
          indices.push_back(b);
          indices.push_back(c);

          indices.push_back(a);          
          indices.push_back(c);
          indices.push_back(d);
        }
      }
#endif

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

      return result;
    }
  }
  return nullptr;
}

float3 SphereMeshGenerator::GetSphereVertex(float alpha, float beta) const
{
  return primitive.radius * MathTools::PolarCoordToVector(alpha, beta) + primitive.position;
}

}; // namespace chaos
