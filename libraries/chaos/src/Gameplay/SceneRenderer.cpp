#include "chaos/Chaos.h"

namespace chaos
{
	void SceneRenderer::DisplayScene(SceneNode* root_node, GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		if (root_node != nullptr)
		{
			// set a reference on the local_to_world transform
			GPUProgramProviderChain main_uniform_provider(uniform_provider);
			main_uniform_provider.AddVariableReference("local_to_world", local_to_world);
			// start the rendering recursion
			DisplayNode(root_node, renderer, uniform_provider, render_params);
		}
	}

	void SceneRenderer::DisplayNode(SceneNode* node, GPURenderer* renderer, GPUProgramProviderInterface const * uniform_provider, GPURenderParams const& render_params)
	{
		assert(node != nullptr);

		// node is invisible and so are its children
		if (!node->PrepareDisplay(renderer, uniform_provider, render_params))
			return;
		// store transformation matrix (this is usefull while we use a VariableReference in ProviderChain)
		glm::mat4 previous_local_to_world = local_to_world;
		// update transformation matrix
		local_to_world = local_to_world * node->GetLocalToParent();
		// display node and its children
		node->Display(renderer, uniform_provider, render_params);
		for (auto& child : node->ChildrenNodes())
			DisplayNode(child.get(), renderer, uniform_provider, render_params);
		// restore transformation matrix
		local_to_world = previous_local_to_world;
	}


	
}; // namespace chaos

