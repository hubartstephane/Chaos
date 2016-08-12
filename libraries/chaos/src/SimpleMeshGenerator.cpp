#include <chaos/SimpleMeshGenerator.h>
#include <chaos/GLTools.h>

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

SimpleMesh * QuadMeshGenerator::GenerateMesh() const
{
  SimpleMesh * result = new SimpleMesh();
  if (result != nullptr)
  {
    GLuint va = 0;
    GLuint vb = 0;
    GLuint ib = 0;

    if (GLTools::GenerateVertexAndIndexBuffers(&va, &vb, &ib))
    {
      result->declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);

      MeshPrimitive primitive;
      primitive.count             = sizeof(triangles) / sizeof(triangles[0]);
      primitive.indexed           = true;
      primitive.primitive_type    = GL_TRIANGLES;
      primitive.start             = 0;
      primitive.base_vertex_index = 0;
      result->primitives.push_back(primitive); 

      result->vertex_array  = new VertexArray(va);
      result->vertex_buffer = new VertexBuffer(vb);
      result->index_buffer  = new IndexBuffer(ib);

      // fill the buffers
      glm::vec3 hs = glm::vec3(box.half_size.x, box.half_size.y, 1.0f);
      glm::vec3 p  = glm::vec3(box.position.x, box.position.y,  0.0f);

      int const count = sizeof(vertices) / sizeof(vertices[0]); 

      glm::vec3 final_vertices[count]; 
      for (int i = 0; i < count; ++i)
        final_vertices[i] = vertices[i] * hs + p;

      glNamedBufferData(vb, sizeof(glm::vec3) * count, final_vertices, GL_STATIC_DRAW);
      glNamedBufferData(ib, sizeof(triangles), triangles, GL_STATIC_DRAW);

      // initialize the vertex array
      result->FinalizeBindings();
    }
    else
    {
      delete(result); 
      result = nullptr;    
    }  
  }
  return result;
}

SimpleMesh * CubeMeshGenerator::GenerateMesh() const
{
  SimpleMesh * result = new SimpleMesh();
  if (result != nullptr)
  {
    GLuint va = 0;
    GLuint vb = 0;
    GLuint ib = 0;

    if (GLTools::GenerateVertexAndIndexBuffers(&va, &vb, &ib))
    {
      result->declaration.Push(chaos::SEMANTIC_POSITION, 0, chaos::TYPE_FLOAT3);
      if (with_normals)
        result->declaration.Push(chaos::SEMANTIC_NORMAL, 0, chaos::TYPE_FLOAT3);

      MeshPrimitive primitive;
      primitive.count             = sizeof(triangles) / sizeof(triangles[0]); // number of triangles does not depends on NORMAL presence
      primitive.indexed           = true;
      primitive.primitive_type    = GL_TRIANGLES;
      primitive.start             = 0;
      primitive.base_vertex_index = 0;
      result->primitives.push_back(primitive); 

      result->vertex_array  = new VertexArray(va);
      result->vertex_buffer = new VertexBuffer(vb);
      result->index_buffer  = new IndexBuffer(ib);

      // resize the mesh      
      if (with_normals)
      {
        int const count = sizeof(vertices_with_normals) / sizeof(vertices_with_normals[0]); // number of vertex * number of component

        glm::vec3 final_vertices[count];
        for (int i = 0 ; i < count / 2 ; ++i)
        {        
          final_vertices[i * 2]     = vertices_with_normals[i * 2] * box.half_size + box.position; // resize position
          final_vertices[i * 2 + 1] = vertices_with_normals[i * 2 + 1];    // copy normal
        }

        glNamedBufferData(vb, sizeof(glm::vec3) * count, final_vertices, GL_STATIC_DRAW);
        glNamedBufferData(ib, sizeof(triangles_with_normals), triangles_with_normals, GL_STATIC_DRAW);
      }
      else
      {
        int const count = sizeof(vertices) / sizeof(vertices[0]);

        glm::vec3 final_vertices[count];
        for (int i = 0; i < count; ++i)
          final_vertices[i] = vertices[i] * box.half_size + box.position;

        glNamedBufferData(vb, sizeof(glm::vec3) * count, final_vertices, GL_STATIC_DRAW);
        glNamedBufferData(ib, sizeof(triangles), triangles, GL_STATIC_DRAW);
      }
         
      // initialize the vertex array
      result->FinalizeBindings(); 
    }
    else
    {
      delete(result); 
      result = nullptr;    
    }  
  }
  return result;
}

}; // namespace chaos
