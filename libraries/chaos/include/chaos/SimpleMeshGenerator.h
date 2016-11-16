#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/SimpleMesh.h>
#include <chaos/MemoryBufferWriter.h>


namespace chaos
{

  /**
   * A class to describe requirement for a mesh
   */

class MeshGenerationRequirement
{
public:

  /** test whether the requirement is valid */
  bool IsValid() const;

public: 

  /** size of a vertex */
	int vertex_size{0};
  /** number of vertices required */
  int vertices_count{0};
  /** number of indices required */
  int indices_count{0};
};

  /**
   * SimpleMeshGeneratorProxy : an object that is responsible for generating the mesh data
   */

class SimpleMeshGeneratorProxy
{
public:

  /** the destructor */
  virtual ~SimpleMeshGeneratorProxy(){}
  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const = 0;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const = 0;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const = 0;
};

  /**
   * A base class for mesh generators
   */

class SimpleMeshGenerator
{
  friend class MultiMeshGenerator;

public:

  /** generation function */
  boost::intrusive_ptr<SimpleMesh> GenerateMesh() const;

protected:

  /** generate a proxy */
  virtual SimpleMeshGeneratorProxy * CreateProxy() const = 0;
};

/**
 * QuadMeshGenerator : help defines mesh as simple quad
 */

class QuadMeshGenerator : public SimpleMeshGenerator
{
  friend class QuadMeshGeneratorProxy;

public:

  /** constructor */
  QuadMeshGenerator(box2 const & in_primitive): 
    primitive(in_primitive) {}

  /** generate a proxy */
  virtual SimpleMeshGeneratorProxy * CreateProxy() const;
 
protected:

  /** the box to generate */
  box2 primitive;
};

class QuadMeshGeneratorProxy : public SimpleMeshGeneratorProxy
{
public:

  /** the constructor */
  QuadMeshGeneratorProxy(QuadMeshGenerator const & in_generator): generator(in_generator){}
  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const override;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const override;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

protected:

  /** the generator */
  QuadMeshGenerator generator;

  /** the vertices defining a face facing planes inside [-1, +1] */
  static glm::vec3 const vertices[4];
  /** the triangles indices defining a face facing planes */
  static GLuint const triangles[6];
};

/**
 * CubeMeshGenerator : help defines cube mesh 
 */

class CubeMeshGenerator : public SimpleMeshGenerator
{

  friend class CubeMeshGeneratorProxy;

public:

  /** constructor */
  CubeMeshGenerator(box3 const & in_primitive):
    primitive(in_primitive){}

  /** generate a proxy */
  virtual SimpleMeshGeneratorProxy * CreateProxy() const;

protected:

  /** the box to generate */
  box3 primitive;
};

/**
 * CubeMeshGeneratorProxy : proxy for CubeMeshGenerator
 */

class CubeMeshGeneratorProxy : public SimpleMeshGeneratorProxy
{
public:

  /** the constructor */
  CubeMeshGeneratorProxy(CubeMeshGenerator const & in_generator) : generator(in_generator) {}
  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const override;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const override;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

protected:

  /** the generator */
  CubeMeshGenerator generator;

  /** the vertices defining a cube */
  static glm::vec3 const vertices[24 * 2];
  /** the triangles defining a cube */
  static GLuint const triangles[36];
};


/**
* SphereMeshGenerator : help defines mesh as simple sphere
*/

class SphereMeshGenerator : public SimpleMeshGenerator
{

  friend class SphereMeshGeneratorProxy;

public:

  /** constructor */
  SphereMeshGenerator(sphere3 const & in_primitive, int in_subdivisions) :
    primitive(in_primitive),
    subdivisions(in_subdivisions){}

  /** generate a proxy */
  virtual SimpleMeshGeneratorProxy * CreateProxy() const;

protected:

  /** the sphere to generate */
  sphere3 primitive;
  /** number of subdivisions */
  int subdivisions;
};

/**
 * SphereMeshGeneratorProxy : proxy for SphereMeshGenerator
 */

class SphereMeshGeneratorProxy : public SimpleMeshGeneratorProxy
{
public:

  /** the constructor */
  SphereMeshGeneratorProxy(SphereMeshGenerator const & in_generator) : generator(in_generator) {}
  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const override;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const override;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

protected:

  /** get a vertex on the sphere from polar angle */
  void InsertVertex(MemoryBufferWriter & vertices_writer, float alpha, float beta) const;

protected:

  /** the generator */
  SphereMeshGenerator generator;
};


}; // namespace chaos

