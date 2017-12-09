#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/GeometryFramework.h>
#include <chaos/SimpleMesh.h>
#include <chaos/ReferencedObject.h>
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
   * SimpleMeshGenerator : an object that is responsible for generating the mesh data
   */

class SimpleMeshGenerator : public ReferencedObject
{
public:

  /** the destructor */
  virtual ~SimpleMeshGenerator() = default;

  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const = 0;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const = 0;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const = 0;

  /** generation function */
  boost::intrusive_ptr<SimpleMesh> GenerateMesh() const;
};

/**
 * QuadMeshGenerator : help defines mesh as simple quad
 */

class QuadMeshGenerator : public SimpleMeshGenerator
{

public:

  /** constructor */
  QuadMeshGenerator(box2 const & in_primitive): 
    primitive(in_primitive) {}

  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const override;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const override;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;
 
protected:

  /** the box to generate */
  box2 primitive;

  /** the vertices defining a face facing planes inside [-1, +1] */
  static glm::vec3 const vertices[4];
  /** the triangles indices defining a face facing planes */
  static GLuint const triangles[6];
};

/**
* TriangleMeshGenerator : help defines mesh as simple traingle
*/

class TriangleMeshGenerator : public SimpleMeshGenerator
{

public:

  /** constructor */
  TriangleMeshGenerator(triangle3 const & in_primitive) :
    primitive(in_primitive) {}

  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const override;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const override;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

protected:

  /** the box to generate */
  triangle3 primitive;
};

/**
 * CubeMeshGenerator : help defines cube mesh 
 */

class CubeMeshGenerator : public SimpleMeshGenerator
{

public:

  /** constructor */
  CubeMeshGenerator(box3 const & in_primitive):
    primitive(in_primitive){}

  /** get requirement */
  virtual MeshGenerationRequirement GetRequirement() const override;
  /** get the vertex declaration */
  virtual void GenerateVertexDeclaration(VertexDeclaration & declaration) const override;
  /** get the mesh data */
  virtual void GenerateMeshData(std::vector<MeshPrimitive> & primitives, MemoryBufferWriter & vertices_writer, MemoryBufferWriter & indices_writer) const override;

protected:

  /** the box to generate */
  box3 primitive;

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

public:

  /** constructor */
  SphereMeshGenerator(sphere3 const & in_primitive, int in_subdivisions) :
    primitive(in_primitive),
    subdivisions(in_subdivisions){}

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

  /** the sphere to generate */
  sphere3 primitive;
  /** number of subdivisions */
  int subdivisions;
};

}; // namespace chaos

