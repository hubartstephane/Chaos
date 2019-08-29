#include <chaos/MyFbxImporter.h>

namespace chaos
{

	bool MyFbxImporter::DoImportSceneSkeletals(FbxScene * myScene)
	{
		FbxNode * root = myScene->GetRootNode();
		return DoImportSceneSkeletals(root, 0);
	}

	bool MyFbxImporter::DoImportSceneSkeletals(FbxNode * node, int level)
	{
		if (node == nullptr)
			return true;

		FbxSkeleton * skeleton = node->GetSkeleton();
		if (skeleton != nullptr)
			return DoImportSkeletonNode(skeleton, node, level);

		return DoImportSceneSkeletalsChildren(node, level);
	}

	bool MyFbxImporter::DoImportSceneSkeletalsChildren(FbxNode * node, int level)
	{
		bool result = true;
		// recursive enumeration
		int count = node->GetChildCount();
		for (int i = 0; (i < count) && (result); ++i)
			result &= DoImportSceneSkeletals(node->GetChild(i), level + 1);
		return result;
	}

	bool MyFbxImporter::DoImportSkeletonNode(FbxSkeleton * skeleton, FbxNode * node, int level)
	{
		assert(skeleton != nullptr);
		assert(node     != nullptr);

		size_t previous_skeleton = current_skeleton_index;
		size_t previous_bone     = current_bone_index;

		// create a root if necessary
		if (skeleton->IsSkeletonRoot())
		{
			assert(current_skeleton_index == std::numeric_limits<size_t>::max()); // there must not be a skeletal beeing created
			assert(previous_bone          == std::numeric_limits<size_t>::max()); // there must not be a skeletal beeing created

			current_skeleton_index = output->skeleton_defs.size();
			current_bone_index     = std::numeric_limits<size_t>::max();

			GPUSkeletonHierarchyDef * def = new GPUSkeletonHierarchyDef;
			def->name = skeleton->GetName();
			output->skeleton_defs.push_back(def);    

			previous_bone = std::numeric_limits<size_t>::max();
		}

		assert(current_skeleton_index != std::numeric_limits<size_t>::max());

		GPUSkeletonHierarchyDef * def = output->skeleton_defs[current_skeleton_index];

		GPUSkeletonHierarchyBoneDef bone_def;
		bone_def.children_count =  0;
		bone_def.first_child    = -1;
		bone_def.bone_sibling   = -1; // no brother node for moment

		if (current_bone_index == std::numeric_limits<size_t>::max())
		{
			bone_def.parent_bone = -1; // no parent, this is root
		}
		else
		{
			bone_def.parent_bone = (int)current_bone_index;

			GPUSkeletonHierarchyBoneDef & parent = def->bone_data[bone_def.parent_bone];
			parent.children_count++;
			if (parent.first_child < 0)
				parent.first_child = (int)def->bone_data.size(); // first child beeing inserted
			else
			{
				int child = parent.first_child;
				while (def->bone_data[child].bone_sibling >= 0)
					child = def->bone_data[child].bone_sibling;
				def->bone_data[child].bone_sibling = (int)def->bone_data.size(); // link this new node to previous brother
			}
		}

		current_bone_index = def->bone_data.size(); // now work with child of this bone

		def->bone_names.push_back(node->GetName());
		def->bone_data.push_back(bone_def);

		bool result = DoImportSceneSkeletalsChildren(node, level);
		current_skeleton_index = previous_skeleton;
		current_bone_index     = previous_bone;
		return result;
	}


}; // namespace chaos
