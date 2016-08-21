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

  // compute the final requirement
  int vertex_buffer_size = 0;
  int index_buffer_size  = 0;

  for (auto const it : generators)
  {
    MeshGenerationRequirement requirement = it.first->GetRequirement();
    if (!requirement.IsValid())
      return false;

    vertex_buffer_size += requirement.vertex_size * requirement.vertices_count;
    index_buffer_size  += requirement.indices_count * sizeof(GLuint);
  }

  // create a vertex buffer and an index buffer to be shared among all meshes
  boost::intrusive_ptr<VertexBuffer> vertex_buffer;
  boost::intrusive_ptr<IndexBuffer>  index_buffer;

  boost::intrusive_ptr<VertexBuffer> * vb_ptr = (vertex_buffer_size > 0)? &vertex_buffer : nullptr;
  boost::intrusive_ptr<IndexBuffer>  * ib_ptr = (index_buffer_size  > 0)? &index_buffer  : nullptr;

  if (!GLTools::GenerateVertexAndIndexBuffersObject(nullptr, vb_ptr, ib_ptr))
    return false;

  // allocate and map the buffers
  void * vertices = (vertex_buffer_size <= 0)? nullptr : glMapNamedBuffer(vertex_buffer->GetResourceID(), 0);
  void * indices  = (index_buffer_size  <= 0)? nullptr : glMapNamedBuffer(index_buffer->GetResourceID(), 0);


  MemoryBufferWriter vertices_writer(vertices, vertex_buffer_size);
  MemoryBufferWriter indices_writer(indices,   index_buffer_size);

  // generate the meshes
  for (auto generator : generators)
  {
    SimpleMeshGeneratorProxy * proxy = generator.first;
    if (proxy == nullptr)
      return false;

    boost::intrusive_ptr<SimpleMesh> new_mesh = new SimpleMesh;
    if (new_mesh == nullptr)
      return false;

    proxy->GenerateVertexDeclaration(new_mesh->declaration);


  }




#ifdef _DEBUG
  size_t remaining_vertex_buffer_size = vertices_writer.GetRemainingBufferSize();
  size_t remaining_index_buffer_size = indices_writer.GetRemainingBufferSize();

  assert(remaining_vertex_buffer_size == 0);
  assert(remaining_index_buffer_size == 0);
#endif


  return true;
}

}; // namespace chaos
