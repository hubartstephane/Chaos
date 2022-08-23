#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	SceneNode::~SceneNode()
	{
		if (parent_node != nullptr)
			parent_node->RemoveChildNode(this);
	}

	glm::mat4 const & SceneNode::GetLocalToParent() const
	{
		if (cache_state & INVALID_LOCAL_TO_PARENT)
		{
			local_to_parent = transform.GetLocalToParent();
			cache_state &= ~INVALID_LOCAL_TO_PARENT;
		}
		return local_to_parent;
	}

	glm::mat4 const & SceneNode::GetParentToLocal() const
	{
		if (cache_state & INVALID_PARENT_TO_LOCAL)
		{
			parent_to_local = transform.GetParentToLocal();
			cache_state &= ~INVALID_PARENT_TO_LOCAL;
		}
		return parent_to_local;
	}

	glm::mat4 SceneNode::GetWorldToLocal() const
	{
		glm::mat4 result = GetParentToLocal();
		
		SceneNode const * node = parent_node.get();
		while (node != nullptr)
		{
			result = result * node->GetParentToLocal();
			node = node->parent_node.get();
		}
		return result;
	}

	glm::mat4 SceneNode::GetLocalToWorld() const
	{
		glm::mat4 result = GetLocalToParent();

		SceneNode const* node = parent_node.get();
		while (node != nullptr)
		{
			result = node->GetLocalToParent() * result;
			node = node->parent_node.get();
		}
		return result;
	}


	void SceneNode::SetPosition(glm::vec2 const& in_position)
	{
		transform.position = in_position;
		cache_state |= (INVALID_LOCAL_TO_PARENT | INVALID_PARENT_TO_LOCAL);
	}

	void SceneNode::SetScale(glm::vec2 const & in_scale)
	{
		transform.scale = in_scale;
		cache_state |= (INVALID_LOCAL_TO_PARENT | INVALID_PARENT_TO_LOCAL);
	}

	void SceneNode::SetRotator(float in_rotator)
	{
		transform.rotator = in_rotator;
		cache_state |= (INVALID_LOCAL_TO_PARENT | INVALID_PARENT_TO_LOCAL);
	}

	
	void SceneNode::AddChildNode(SceneNode* in_child)
	{
		assert(in_child != nullptr);
		assert(in_child->parent_node == nullptr);

		child_nodes.push_back(in_child);
	}
	
	void SceneNode::RemoveChildNode(SceneNode* in_child)
	{
		assert(in_child != nullptr);
		assert(in_child->parent_node == this);

		size_t count = child_nodes.size();
		for (size_t i = 0 ; i < count ; ++i)
		{
			if (child_nodes[i] == in_child)
			{
				child_nodes[i]->parent_node = nullptr;
				child_nodes.erase(child_nodes.begin() + i); // may call destructor
				return;
			}
		}
	}

	int SceneNode::DoDisplay(GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{

		return 0;
	}
	
}; // namespace chaos

