#include "chaos/Chaos.h"

namespace chaos
{
	//
	// GPUProgramProviderInterface implementation
	//

	bool GPUProgramProviderInterface::ProcessAction(char const* name, GPUProgramAction& action) const
	{
		GPUProgramProviderExecutionData execution_data(name, action);
		execution_data.top_provider = this;

		// search for explict first ...
		execution_data.pass_type = GPUProgramProviderPassType::EXPLICIT;
		if (DoProcessAction(execution_data))
			return true;
		// ... then use deduced rules
		execution_data.pass_type = GPUProgramProviderPassType::DEDUCED;
		if (DoProcessAction(execution_data))
			return true;
		// ... finally accept any fallback values
		execution_data.pass_type = GPUProgramProviderPassType::FALLBACK;
		if (DoProcessAction(execution_data))
			return true;
		return false;
	}

	bool GPUProgramProviderInterface::BindUniform(GLUniformInfo const& uniform) const
	{
		GPUProgramSetUniformAction action(uniform);
		return ProcessAction(uniform.name.c_str(), action);
	}

	bool GPUProgramProviderInterface::BindAttribute(GLAttributeInfo const& attribute) const
	{
		GPUProgramSetAttributeAction action(attribute);
		return ProcessAction(attribute.name.c_str(), action);
	}

	bool GPUProgramProviderInterface::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		return false;
	}

	//
	// GPUProgramProviderTexture implementation
	//

	bool GPUProgramProviderTexture::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		if (execution_data.Match(handled_name.c_str(), pass_type))
			return execution_data.Process(value.get(), this); // This is the only place where the provider is required (for texture replacement)
		return false;
	}

	//
	// GPUProgramProvider implementation
	//

	void GPUProgramProvider::Clear()
	{
		children_providers.clear();
	}

	void GPUProgramProvider::AddProvider(GPUProgramProviderBase * provider)
	{
		if (provider != nullptr)
			children_providers.push_back(provider);
	}

	bool GPUProgramProvider::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		// handle children providers
		size_t count = children_providers.size();
		for (size_t i = count; i > 0; --i)
		{
			size_t index = i - 1;
			if (children_providers[index]->DoProcessAction(execution_data))
				return true;
		}
		// failure
		return false;
	}

	//
	// GPUProgramProviderCustom implementation
	//

	bool GPUProgramProviderCustom::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const
	{
		// use the functor
		if (process_func)
			if (process_func(execution_data))
				return true;
		// failure
		return false;
	}

	//
	// GPUProgramProviderChain implementation
	//

	bool GPUProgramProviderChain::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		// use variables inside this provider
		if (GPUProgramProvider::DoProcessAction(execution_data))
			return true;
		// uses the default entries
		for (GPUProgramProviderChainEntry const & entry : entries)
		{
			if (entry.process_func)
			{
				if (entry.process_func(execution_data))
					return true;
			}
			else if (entry.provider != nullptr)
			{
				if (entry.provider->DoProcessAction(execution_data))
					return true;
			}
		}
		return false;
	}

	//
	// GPUProgramProvideDeducedTransformations implementation
	//

	bool GPUProgramProviderCommonTransforms::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		// some fallbacks
		static const glm::mat4 identity = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

		if (execution_data.Match("local_to_world", GPUProgramProviderPassType::FALLBACK))
		{
			return execution_data.Process(identity);
		}
		if (execution_data.Match("world_to_local", GPUProgramProviderPassType::FALLBACK))
		{
			return execution_data.Process(identity);
		}
		if (execution_data.Match("world_to_camera", GPUProgramProviderPassType::FALLBACK))
		{
			return execution_data.Process(identity);
		}
		if (execution_data.Match("camera_to_world", GPUProgramProviderPassType::FALLBACK))
		{
			return execution_data.Process(identity);
		}

		// deduced: local_to_world = inverse(world_to_local)
		if (auto lock1 = execution_data.CanDeduce("local_to_world"))
		{
			glm::mat4 world_to_local;
			if (execution_data.GetValue("world_to_local", world_to_local))
				return execution_data.Process(glm::inverse(world_to_local));
		}
		// deduced: world_to_local = inverse(local_to_world)
		if (auto lock2 = execution_data.CanDeduce("world_to_local"))
		{
			glm::mat4 local_to_world;
			if (execution_data.GetValue("local_to_world", local_to_world))
				return execution_data.Process(glm::inverse(local_to_world));
		}
		// deduced: camera_to_world = inverse(world_to_camera)
		if (auto lock = execution_data.CanDeduce("camera_to_world"))
		{
			glm::mat4 world_to_camera;
			if (execution_data.GetValue("world_to_camera", world_to_camera))
				return execution_data.Process(glm::inverse(world_to_camera));
		}
		// deduced: world_to_camera = inverse(camera_to_world)
		if (auto lock = execution_data.CanDeduce("world_to_camera"))
		{
			glm::mat4 camera_to_world;
			if (execution_data.GetValue("camera_to_world", camera_to_world))
				return execution_data.Process(glm::inverse(camera_to_world));
		}

		// deduced: local_to_camera = world_to_camera * local_to_world
		if (auto lock = execution_data.CanDeduce("local_to_camera"))
		{
			glm::mat4 local_to_world;
			glm::mat4 world_to_camera;
			if (execution_data.GetValue("local_to_world", local_to_world) && execution_data.GetValue("world_to_camera", world_to_camera))
			{
				return execution_data.Process(world_to_camera * local_to_world);
			}
		}
		// deduced: camera_to_local = inverse(local_to_camera)
		if (auto lock = execution_data.CanDeduce("camera_to_local"))
		{
			glm::mat4 local_to_camera;
			if (execution_data.GetValue("local_to_camera", local_to_camera))
				return execution_data.Process(glm::inverse(local_to_camera));

		}
		return GPUProgramProvider::DoProcessAction(execution_data);
	}

}; // namespace chaos
