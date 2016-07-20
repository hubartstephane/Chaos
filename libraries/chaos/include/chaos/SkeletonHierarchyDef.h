#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

/**
 * SkeletonHierarchyBoneDef : defines a bone in the hierarchy
 */

class SkeletonHierarchyBoneDef
{
public:

  /** index of the parent bone in the bone_data vector */
  int parent_bone;
  /** index of the first child in the bone_data vector */
  int first_child;
  /** number of children */
  int children_count;
  /** the next child index */
  int bone_sibling;
};

/**
* SkeletonHierarchyDef : defines the chaine of bones
*/

class SkeletonHierarchyDef
{
public:

  /** the name of the hierarchy */
  std::string name;
  /** the name of all bones */
  std::vector<std::string> bone_names;
  /** the hierarchy itself */
  std::vector<SkeletonHierarchyBoneDef> bone_data;
};

}; // namespace chaos
