#include <chaos/MultiMeshGenerator.h>

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
  generators.push_back(std::make_pair(generator.CreateProxy(), &target_ptr));
}

void MultiMeshGenerator::Clean()
{
  for (auto & it : generators)
    delete (it.first);
  generators.clear(); // destroy the intrusive_ptr
}

bool MultiMeshGenerator::GenerateMeshes() const
{
  return true;
}

}; // namespace chaos
