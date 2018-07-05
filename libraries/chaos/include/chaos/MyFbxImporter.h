#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/SimpleMesh.h>
#include <chaos/GPUVertexDeclaration.h>
#include <chaos/Buffer.h>
#include <chaos/SkeletonHierarchyDef.h>

namespace chaos
{

/**
 * MyFbxImporterOutput : a class to hold all data results from the importation
 */

class MyFbxImporterOutput
{
public:

  /** Destructor */
  virtual ~MyFbxImporterOutput();

  /** Clean the output */
  virtual void Clean();

public:

  /** the meshes that have been read */
  std::vector<SimpleMesh *> meshes;
  /** the skeleton hierarchies that have been read */
  std::vector<SkeletonHierarchyDef *> skeleton_defs;
};

/**
 * MyFbxImporterParams : parameters for the importation
 */

class MyFbxImporterParams
{
public:

  /** the constructor */
  MyFbxImporterParams();

  /** whether the scene hierarchy is to be imported */
  bool import_scene_hierarchy;

  /** whether the meshes are to be imported */
  bool import_meshes;
  /** whether the skeletons are to be imported */
  bool import_skeletons;
  /** whether the materials are to be imported */
  bool import_materials;
  /** whether the materials are to be imported */
  bool import_textures;
  /** whether the characters are to be imported */
  bool import_characters;
  /** whether the character poses are to be imported */
  bool import_character_poses;
  /** whether the poses are to be imported */
  bool import_poses;

  /** whether the camera nodes are to be imported */
  bool import_camera_nodes;
  /** whether the light nodes are to be imported */
  bool import_light_nodes;
  /** whether the marker nodes are to be imported */
  bool import_marker_nodes;
  /** whether the geometry nodes are to be imported */
  bool import_geometry_nodes;
  /** whether the skeleton nodes are to be imported */
  bool import_skeleton_nodes;

  /** during mesh importation, should vertices be merged whenever possible */
  bool remove_duplicated_vertex;
  /** a matrix to apply on position vertex for permutation, rescale */
  glm::mat4 vertex_transforms;
};


/**
 * MyFbxImporter : a base class to import FBX file
 */

class MyFbxImporter
{
  /** information for skinning */
  typedef std::vector<std::pair<int32_t, float>> SkinDataType;
  /** a table that give an association CTRL_POINT => array of [BONE_INDEX, BONE_WEIGHT] */
  typedef std::map<int, SkinDataType> BoneTableType;

public:

  /** constructor */
  MyFbxImporter();

  /** Main method to import a scene */
  virtual bool ImportScene(boost::filesystem::path const & model_path, MyFbxImporterOutput & output, MyFbxImporterParams const & in_params = MyFbxImporterParams());

protected:

  /** Internal method to import a scene */
  virtual bool ImportScene(FbxManager * lSdkManager, FbxImporter * lImporter);
  /** Internal methods to import a scene */
  virtual bool DoImportScene(FbxScene * myScene);
  /** Internal methods to import the skeletal of the scene */
  virtual bool DoImportSceneSkeletals(FbxScene * myScene);
  /** Internal methods to import the skeletal of the scene */
  virtual bool DoImportSceneSkeletals(FbxNode * node, int level);
  /** Internal methods to import the skeletal of the scene */
  virtual bool DoImportSceneSkeletalsChildren(FbxNode * node, int level);

  /** Internal methods to import resources */
  virtual bool DoImportSceneResources(FbxScene * myScene);
  /** Internal methods to import the hierarchy */
  virtual bool DoImportSceneHierarchy(FbxScene * myScene);
  /** Internal method to import a node in a scene */
  virtual bool DoImportSceneHierarchy(FbxNode * node, int level);

  /** import one character */
  virtual bool DoImportCharacter(FbxCharacter * character);
  /** import one pose */
  virtual bool DoImportPose(FbxPose * character);
  /** import one character pose */
  virtual bool DoImportCharacterPose(FbxCharacterPose * character_pose);
  /** import one material */
  virtual bool DoImportMaterial(FbxSurfaceMaterial * material);
  /** import one texture */
  virtual bool DoImportTexture(FbxTexture * texture);
  /** import one geometry (mesh or skeleton) */
  virtual bool DoImportGeometry(FbxGeometry * geometry);
  /** import one mesh */
  virtual bool DoImportMesh(FbxMesh * mesh);

  /** import the children nodes */
  bool DoImportSceneHierarchyChildren(FbxNode * node, int level);

  /** import one geometry */
  virtual bool DoImportGeometryNode(FbxGeometry * mesh, FbxNode * node, int level);
  /** import one marker */
  virtual bool DoImportMarkerNode(FbxMarker * marker, FbxNode * node, int level);
  /** import one skeleton */
  virtual bool DoImportSkeletonNode(FbxSkeleton * skeleton, FbxNode * node, int level);
  /** import one camera */
  virtual bool DoImportCameraNode(FbxCamera * camera, FbxNode * node, int level);
  /** import one light */
  virtual bool DoImportLightNode(FbxLight * light, FbxNode * node, int level);

  /** internal method to create a mesh */
  SimpleMesh * DoCreateMesh(GPUVertexDeclaration const & vertex_declaration, Buffer<char> vertices, std::vector<int> const & index_buffer);

  /** get assoication table, CTRL_POINT => BONE INFO */
  static BoneTableType GetBoneData(FbxMesh * mesh);
  /** get the max number of bone for a given CTRL POINT */
  static size_t GetMaxBoneCount(BoneTableType const & table);


protected:

  /** the parameters for the importation */
  MyFbxImporterParams params;
  /** the result of the importation */
  MyFbxImporterOutput * output;
  /** the currently constructed skeleton hierarchy */
  size_t current_skeleton_index;
  /** the currently constructed bone */
  size_t current_bone_index;

};

}; // namespace chaos
