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

void MultiMeshGenerator::AddGenerator(SimpleMeshGenerator * generator, boost::intrusive_ptr<SimpleMesh> & target_ptr)
{
  assert(generator != nullptr);

  boost::intrusive_ptr<SimpleMeshGenerator> generator_ptr = generator;

  generators.push_back(std::make_pair(generator_ptr, &target_ptr));
}

void MultiMeshGenerator::Clean()
{
  generators.clear(); // destroy the intrusive_ptr
}

bool MultiMeshGenerator::GenerateMeshes() const
{  
  if (generators.size() == 0)
    return true;

  // compute the final requirements
  int vb_size = 0;
  int ib_size = 0;

  for (auto const it : generators)
  {
    MeshGenerationRequirement requirement = it.first->GetRequirement();
    if (!requirement.IsValid())
      return false;

    vb_size += requirement.vertex_size * requirement.vertices_count;
    ib_size += requirement.indices_count * sizeof(GLuint);
  }

  // create a vertex buffer and an index buffer to be shared among all meshes
  boost::intrusive_ptr<VertexBuffer> vertex_buffer;
  boost::intrusive_ptr<IndexBuffer>  index_buffer;

  boost::intrusive_ptr<VertexBuffer> * vb_ptr = (vb_size > 0)? &vertex_buffer : nullptr;
  boost::intrusive_ptr<IndexBuffer>  * ib_ptr = (ib_size > 0)? &index_buffer  : nullptr;

  if (GLTools::GenerateVertexAndIndexBuffersObject(nullptr, vb_ptr, ib_ptr))
  {
    GLuint vb = (vb_size > 0)? vertex_buffer->GetResourceID() : 0;
    GLuint ib = (ib_size > 0)? index_buffer->GetResourceID()  : 0;

    // allocate buffer for vertices and indices
    std::pair<char *, GLuint *> mapping;
    if (GLTools::MapBuffers(vb, ib, vb_size, ib_size, mapping))
    {
      // generate the indices and the vertices
      MemoryBufferWriter vertices_writer(mapping.first, vb_size);
      MemoryBufferWriter indices_writer(mapping.second, ib_size);

      for (auto const it : generators)
      {
        MeshGenerationRequirement requirement = it.first->GetRequirement();

        int written_vertices_count = vertices_writer.GetWrittenCount();
        int written_indices_count  = indices_writer.GetWrittenCount();

        boost::intrusive_ptr<SimpleMesh> mesh = new SimpleMesh; // generate the mesh
        if (mesh != nullptr)
        {
          GLuint vertex_array = 0;
          glCreateVertexArrays(1, &vertex_array); // generate a vertex array per mesh
          if (vertex_array != 0)
          {
            mesh->vertex_array = new VertexArray(vertex_array);

            if (requirement.vertices_count > 0)
              mesh->vertex_buffer = vertex_buffer;
            if (requirement.indices_count > 0)
              mesh->index_buffer = index_buffer;

            it.first->GenerateMeshData(mesh->primitives, vertices_writer, indices_writer); // generate the buffers and primitives and declaration
            it.first->GenerateVertexDeclaration(mesh->declaration);

            assert(requirement.vertex_size == mesh->declaration.GetVertexSize());

            mesh->ShiftPrimitivesIndexAndVertexPosition(0, written_indices_count / sizeof(GLuint));  // shift the position of vertices/indices for this mesh

            mesh->FinalizeBindings(written_vertices_count);

            (*it.second) = mesh; // store the mesh as an output          
          }
        }
      }

      assert(vertices_writer.GetRemainingBufferSize() == 0);
      assert(indices_writer.GetRemainingBufferSize() == 0);

      // transfert data to GPU and free memory
      if (vb != 0)
        glUnmapNamedBuffer(vb);
      if (ib != 0)
        glUnmapNamedBuffer(ib);
    }      
  }
  return true;
}

}; // namespace chaos
