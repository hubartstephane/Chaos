#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SimpleMesh.h>

namespace chaos
{

/**
 * FaceFacingQuadMeshDef : help defines mesh as simple quad
 */

class FaceFacingQuadMeshDef
{

public:

  /** generate a mesh for the rectangle */
  static SimpleMesh * CreateMesh(bool centered);
 
  /** the vertices defining a face facing planes inside [0, +1] */
  static glm::vec3 const positive_vertices[4];
  /** the vertices defining a face facing planes inside [-1, +1] */
  static glm::vec3 const centered_vertices[4];
  /** the triangles indices defining a face facing planes */
  static GLuint const triangles[6];
};

/**
 * CubeMeshDef : help defines cube mesh 
 */

class CubeMeshDef
{
public:

  /** generate a mesh for the cube */
  static SimpleMesh * CreateMesh(glm::vec3 const & size, bool with_face_normals);

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

