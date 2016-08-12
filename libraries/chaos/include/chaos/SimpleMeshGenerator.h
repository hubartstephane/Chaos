#pragma once

#include <chaos/GeometryFramework.h>
#include <chaos/StandardHeaders.h>
#include <chaos/SimpleMesh.h>


namespace chaos
{

  /**
   * A base class for mesh generators
   */

class SimpleMeshGenerator
{
public:

  /** destructor */
  virtual ~SimpleMeshGenerator(){}
  /** generation function */
  virtual boost::intrusive_ptr<SimpleMesh> GenerateMesh() const { return nullptr; }
};


/**
 * QuadMeshGenerator : help defines mesh as simple quad
 */

class QuadMeshGenerator : public SimpleMeshGenerator
{

public:

  /** constructor */
  QuadMeshGenerator(box2 const & in_box): 
    box(in_box) {}

  /** generation function */
  virtual boost::intrusive_ptr<SimpleMesh> GenerateMesh() const override;
 
protected:

  /** the box to generate */
  box2 box;

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
public:

  /** constructor */
  CubeMeshGenerator(box3 const & in_box, bool in_with_normals): 
    box(in_box), 
    with_normals(in_with_normals){}

  /** generation function */
  virtual boost::intrusive_ptr<SimpleMesh> GenerateMesh() const override;

protected:

  /** the box to generate */
  box3 box;
  /** whether normals are to be generated */
  bool with_normals;

  /** the vertices defining a cube */
  static glm::vec3 const vertices[8];
  /** the triangles defining a cube */
  static GLuint const triangles[36];

  /** the vertices defining a cube */
  static glm::vec3 const vertices_with_normals[24 * 2];
  /** the triangles defining a cube */
  static GLuint const triangles_with_normals[36];
};

}; // namespace chaos

