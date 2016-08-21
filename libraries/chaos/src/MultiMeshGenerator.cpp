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

void MultiMeshGenerator::AddGenerator(SimpleMeshGenerator const & generator, boost::intrusive_ptr<SimpleMesh> & target_ptr)
{
  SimpleMeshGeneratorProxy * proxy = generator.CreateProxy();
  if (proxy != nullptr)
    generators.push_back(std::make_pair(proxy, &target_ptr));
}

void MultiMeshGenerator::Clean()
{
  for (auto it : generators)
    delete (it.first);
  generators.clear(); // destroy the intrusive_ptr
}

bool MultiMeshGenerator::GenerateMeshes() const
{  
  if (generators.size() == 0)
    return true;

  // compute the final requirements
  int required_vertex_buffer_size = 0;
  int required_index_buffer_size  = 0;

  for (auto const it : generators)
  {
    MeshGenerationRequirement requirement = it.first->GetRequirement();
    if (!requirement.IsValid())
      return false;

    required_vertex_buffer_size += requirement.vertex_size * requirement.vertices_count;
    required_index_buffer_size  += requirement.indices_count * sizeof(GLuint);
  }

  // create a vertex buffer and an index buffer to be shared among all meshes
  boost::intrusive_ptr<VertexBuffer> vertex_buffer;
  boost::intrusive_ptr<IndexBuffer>  index_buffer;

  boost::intrusive_ptr<VertexBuffer> * vb_ptr = (required_vertex_buffer_size > 0)? &vertex_buffer : nullptr;
  boost::intrusive_ptr<IndexBuffer>  * ib_ptr = (required_index_buffer_size  > 0)? &index_buffer  : nullptr;

  if (!GLTools::GenerateVertexAndIndexBuffersObject(nullptr, vb_ptr, ib_ptr))
    return false;

  // allocate and map the buffers
  char   * vertices = nullptr;
  GLuint * indices = nullptr;

  if (required_vertex_buffer_size > 0)
  {
    GLuint buffer_id = vertex_buffer->GetResourceID();
    glNamedBufferData(buffer_id, required_vertex_buffer_size, nullptr, GL_STATIC_DRAW);
    vertices = (char *)glMapNamedBuffer(buffer_id, GL_WRITE_ONLY);
    if (vertices == nullptr)
      goto release_resources;
  }
  if (required_index_buffer_size > 0)
  {
    GLuint buffer_id = index_buffer->GetResourceID();
    glNamedBufferData(buffer_id, required_index_buffer_size, nullptr, GL_STATIC_DRAW);
    indices = (GLuint *)glMapNamedBuffer(buffer_id, GL_WRITE_ONLY);
    if (indices == nullptr)
      goto release_resources;
  }

  // generate the indices and the vertices
  MemoryBufferWriter vertices_writer(vertices, required_vertex_buffer_size);
  MemoryBufferWriter indices_writer(indices, required_index_buffer_size);

  for (auto const it : generators)
  {
    MeshGenerationRequirement requirement = it.first->GetRequirement();

    int written_vertices_count = vertices_writer.GetWrittenCount();
    int written_indices_count = indices_writer.GetWrittenCount();

    boost::intrusive_ptr<SimpleMesh> mesh = new SimpleMesh; // generate the mesh
    if (mesh == nullptr)
      goto release_resources;

    GLuint vertex_array = 0;
    glCreateVertexArrays(1, &vertex_array); // generate a vertex array per mesh
    if (vertex_array == 0)
      goto release_resources;
    mesh->vertex_array = new VertexArray(vertex_array);

    if (requirement.vertices_count > 0)
      mesh->vertex_buffer = vertex_buffer;
    if (requirement.indices_count > 0)
      mesh->index_buffer = index_buffer;
       
    it.first->GenerateMeshData(mesh->primitives, vertices_writer, indices_writer); // generate the buffers and primitives and declaration
    it.first->GenerateVertexDeclaration(mesh->declaration);

    assert(requirement.vertex_size == mesh->declaration.GetVertexSize());

    mesh->ShiftPrimitivesIndexAndVertexPosition(written_vertices_count, written_indices_count);  // shift the position of vertices/indices for this mesh
    
    mesh->FinalizeBindings();

    (*it.second) = mesh; // store the mesh as an output
  }

  assert(vertices_writer.GetRemainingBufferSize() == 0);
  assert(indices_writer.GetRemainingBufferSize() == 0);

  // transfert data to GPU and free memory

release_resources:

  if (vertices != nullptr)
  {
    GLuint buffer_id = vertex_buffer->GetResourceID();
    glUnmapNamedBuffer(buffer_id);
  }
  if (indices != nullptr)
  {
    GLuint buffer_id = index_buffer->GetResourceID();
    glUnmapNamedBuffer(buffer_id);
  }



  return true;
}

}; // namespace chaos
