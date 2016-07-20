#include <chaos/SimpleMeshDef.h>
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

glm::vec3 const FaceFacingQuadMeshDef::positive_vertices[4] =
{
  glm::vec3(0.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 0.0f, 0.0f),
  glm::vec3(1.0f, 1.0f, 0.0f),
  glm::vec3(0.0f, 1.0f, 0.0f)
};

glm::vec3 const FaceFacingQuadMeshDef::centered_vertices[4] =
{
  glm::vec3(-1.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f, -1.0f, 0.0f),
  glm::vec3( 1.0f,  1.0f, 0.0f),
  glm::vec3(-1.0f,  1.0f, 0.0f)
};

GLuint const FaceFacingQuadMeshDef::triangles[6] =
{
  0, 1, 2,  
  0, 2, 3
};

glm::vec3 const CubeMeshDef::vertices[8] =
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

GLuint const CubeMeshDef::triangles[36] =
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


SimpleMesh * FaceFacingQuadMeshDef::CreateMesh(bool centered)
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
      if (centered)
        glNamedBufferData(vb, sizeof(centered_vertices), centered_vertices, GL_STATIC_DRAW);
      else
        glNamedBufferData(vb, sizeof(positive_vertices), positive_vertices, GL_STATIC_DRAW);

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


SimpleMesh * CubeMeshDef::CreateMesh()
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
      glNamedBufferData(vb, sizeof(vertices), vertices, GL_STATIC_DRAW);
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

}; // namespace chaos
