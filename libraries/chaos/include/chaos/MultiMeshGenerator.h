#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SimpleMeshGenerator.h>

namespace chaos
{
  /**
   * MultiMeshGenerator : this class is used to generator multiple mesh in a row, shared vertex and index buffer
   */

class MultiMeshGenerator
{
public:

  /** constructor */
  MultiMeshGenerator();
  /** destructor */
  virtual ~MultiMeshGenerator();

  /** the insertion method */
  void AddGenerator(SimpleMeshGenerator const & generator, boost::intrusive_ptr<SimpleMesh> & target_ptr);
  /** clean all generators */
  void Clean();
  /** generate all meshes */
  bool GenerateMeshes() const;

protected:

  /** the registered element to generate */
  std::vector<std::pair<SimpleMeshGeneratorProxy *, boost::intrusive_ptr<SimpleMesh> *>> generators;   
};


}; // namespace chaos

