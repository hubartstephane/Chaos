#ifdef CHAOS_FORWARD_DECLARATION

namespace chaos
{
	class GPUSkeletonHierarchyBoneDef;
	class GPUSkeletonHierarchyDef;

}; // namespace chaos

#else 

namespace chaos
{
	/**
	* GPUSkeletonHierarchyBoneDef : defines a bone in the hierarchy
	*/

	class GPUSkeletonHierarchyBoneDef
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
	* GPUSkeletonHierarchyDef : defines the chain of bones
	*/

	class GPUSkeletonHierarchyDef
	{
	public:

		/** the name of the hierarchy */
		std::string name;
		/** the name of all bones */
		std::vector<std::string> bone_names;
		/** the hierarchy itself */
		std::vector<GPUSkeletonHierarchyBoneDef> bone_data;
	};

}; // namespace chaos

#endif // CHAOS_FORWARD_DECLARATION


