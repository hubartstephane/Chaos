#include <chaos/Chaos.h>

namespace chaos
{
	glm::mat4 const & SceneNode::GetLocalToParent() const
	{
		if (cache_state & INVALID_LOCAL_TO_PARENT)
		{
			local_to_parent = 
				glm::translate(glm::vec3(position, 0.0f)) * 
				GetRotatorMatrix(rotator) * 
				glm::scale(glm::vec3(scale, 1.0f));

			cache_state &= ~INVALID_LOCAL_TO_PARENT;
		}
		return local_to_parent;
	}

	glm::mat4 const & SceneNode::GetParentToLocal() const
	{
		if (cache_state & INVALID_PARENT_TO_LOCAL)
		{
			parent_to_local =
				glm::scale(glm::vec3(1.0f / scale, 1.0f)) *
				GetRotatorMatrix(-rotator) *
				glm::translate(-glm::vec3(position, 0.0f));

			cache_state &= ~INVALID_PARENT_TO_LOCAL;
		}
		return parent_to_local;
	}


	void SceneNode::SetPosition(glm::vec2 const& in_position)
	{
		position = in_position;
		cache_state |= (INVALID_LOCAL_TO_PARENT | INVALID_PARENT_TO_LOCAL);
	}

	void SceneNode::SetScale(glm::vec2 const & in_scale)
	{
		scale = in_scale;
		cache_state |= (INVALID_LOCAL_TO_PARENT | INVALID_PARENT_TO_LOCAL);
	}

	void SceneNode::SetRotator(float in_rotator)
	{
		rotator = in_rotator;
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
				child_nodes.erase(child_nodes.begin() + i);
				return;
			}
		}
	}

	int SceneNode::DoDisplay(GPURenderer* renderer, GPUProgramProviderBase const* uniform_provider, GPURenderParams const& render_params)
	{

		return 0;
	}
	
}; // namespace chaos

