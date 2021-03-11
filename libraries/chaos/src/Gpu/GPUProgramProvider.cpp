#include <chaos/Chaos.h>

namespace chaos
{
	//
	// GPUProgramProviderDeduceLock implementation
	//

	GPUProgramProviderDeduceLock::GPUProgramProviderDeduceLock(GPUProgramProviderDeduceLock&& src) :
		searched_name(src.searched_name),
		execution_data(src.execution_data)
	{
		src.searched_name = nullptr;
		src.execution_data = nullptr;
	}

	GPUProgramProviderDeduceLock::~GPUProgramProviderDeduceLock()
	{
		if (execution_data != nullptr && searched_name != nullptr)
		{
			auto it = std::find_if(execution_data->deduced_searches->begin(), execution_data->deduced_searches->end(), [searched_name = searched_name](char const* other_name)
			{
				return (StringTools::Strcmp(other_name, searched_name) == 0);
			});
			if (it != execution_data->deduced_searches->end())
			{
				size_t index = it - execution_data->deduced_searches->begin();
				execution_data->deduced_searches->operator[](index) = execution_data->deduced_searches->back();
				execution_data->deduced_searches->pop_back();
			}
		}
	}

	GPUProgramProviderDeduceLock::operator bool() const
	{
		return (searched_name != nullptr) && (execution_data != nullptr);
	}

	//
	// GPUProgramProviderExecutionData implementation
	//

	GPUProgramProviderExecutionData::GPUProgramProviderExecutionData(char const* in_searched_name, GPUProgramAction& in_action, GPUProgramProviderExecutionData const* base_execution) :
		searched_name(in_searched_name),
		action(in_action)
	{
		assert(in_searched_name != nullptr);
		if (base_execution == nullptr)
		{
			deduced_searches = &internal_deduced_searches; // point to our own vector
		}
		else
		{
			top_provider = base_execution->top_provider;
			deduced_searches = base_execution->deduced_searches; // do not point to our internal vector, but on another one's !
		}
	}

	bool GPUProgramProviderExecutionData::Match(char const* other_name) const
	{
		return (StringTools::Strcmp(other_name, searched_name) == 0);
	}

	bool GPUProgramProviderExecutionData::Match(std::string const& other_name) const
	{
		return Match(other_name.c_str());
	}

	bool GPUProgramProviderExecutionData::Process(GPUTexture const* value, GPUProgramProviderBase const* provider) const
	{
		return action.Process(searched_name, value, provider);
	}

	GPUProgramProviderDeduceLock GPUProgramProviderExecutionData::CanDeduce(char const* searched_name) const
	{
		// can only deduce for this pass
		if (pass_type != GPUProgramProviderPassType::DEDUCED)
			return {};
		// is that what we are searching ?
		if (!Match(searched_name))
			return {};
		// already searching this name (avoid infinite recursion, may search in lower priority providers)
		auto it = std::find_if(deduced_searches->begin(), deduced_searches->end(), [searched_name](char const * other_name)
		{ 
			return (StringTools::Strcmp(other_name, searched_name) == 0);
		});
		if (it != deduced_searches->end())
			return {};
		// start a deduce request (no need to deep copy the string)
		deduced_searches->push_back(searched_name);
		// return the lock
		GPUProgramProviderDeduceLock result;
		result.searched_name = searched_name;
		result.execution_data = this;
		return std::move(result); // no copy
	}

	//
	// GPUProgramProviderBase implementation
	//

	bool GPUProgramProviderBase::ProcessAction(char const* name, GPUProgramAction& action) const
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

	bool GPUProgramProviderBase::BindUniform(GLUniformInfo const& uniform) const
	{
		return ProcessAction(uniform.name.c_str(), GPUProgramSetUniformAction(uniform));
	}

	bool GPUProgramProviderBase::BindAttribute(GLAttributeInfo const& attribute) const
	{
		return ProcessAction(attribute.name.c_str(), GPUProgramSetAttributeAction(attribute));
	}
	
	bool GPUProgramProviderBase::DoProcessAction(GPUProgramProviderExecutionData const& execution_data) const 
	{ 
		return false; 
	}

	//
	// GPUProgramProviderTexture implementation
	//

	bool GPUProgramProviderTexture::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		if (execution_data.GetPassType() == pass_type && execution_data.Match(handled_name))
			return execution_data.Process(value.get(), this);
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
		// use the functor
		if (process_func)
			if (process_func(execution_data))
				return true;
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
	// GPUProgramProviderChain implementation
	//

	bool GPUProgramProviderChain::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		// use variables inside this provider
		if (GPUProgramProvider::DoProcessAction(execution_data))
			return true;
		// use fallback provider
		if (fallback_provider != nullptr)
			if (fallback_provider->DoProcessAction(execution_data))
				return true;
		return false;
	}

	//
	// GPUProgramProvideDeducedTransformations implementation
	//

	bool GPUProgramProviderCommonTransforms::DoProcessAction(GPUProgramProviderExecutionData const & execution_data) const
	{
		// some fallbacks
		if (execution_data.GetPassType() == GPUProgramProviderPassType::FALLBACK)
		{
			static const glm::mat4 identity = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

			if (execution_data.Match("local_to_world"))
			{
				return execution_data.Process(identity, this);
			}
			if (execution_data.Match("world_to_local"))
			{
				return execution_data.Process(identity, this);
			}
			if (execution_data.Match("world_to_camera"))
			{
				return execution_data.Process(identity, this);
			}
			if (execution_data.Match("camera_to_world"))
			{
				return execution_data.Process(identity, this);
			}
		}

		// deduced: local_to_world = inverse(world_to_local)
		if (auto lock1 = execution_data.CanDeduce("local_to_world"))
		{
			glm::mat4 world_to_local;
			if (execution_data.GetValue("world_to_local", world_to_local))
				return execution_data.Process(glm::inverse(world_to_local), this);
		}
		// deduced: world_to_local = inverse(local_to_world)
		if (auto lock2 = execution_data.CanDeduce("world_to_local"))
		{
			glm::mat4 local_to_world;
			if (execution_data.GetValue("local_to_world", local_to_world))
				return execution_data.Process(glm::inverse(local_to_world), this);
		}
		// deduced: camera_to_world = inverse(world_to_camera)
		if (auto lock = execution_data.CanDeduce("camera_to_world"))
		{
			glm::mat4 world_to_camera;
			if (execution_data.GetValue("world_to_camera", world_to_camera))
				return execution_data.Process(glm::inverse(world_to_camera), this);
		}
		// deduced: world_to_camera = inverse(camera_to_world)
		if (auto lock = execution_data.CanDeduce("world_to_camera"))
		{
			glm::mat4 camera_to_world;
			if (execution_data.GetValue("camera_to_world", camera_to_world))
				return execution_data.Process(glm::inverse(camera_to_world), this);
		}

		// deduced: local_to_camera = world_to_camera * local_to_world
		if (auto lock = execution_data.CanDeduce("local_to_camera"))
		{
			glm::mat4 local_to_world;
			glm::mat4 world_to_camera;
			if (execution_data.GetValue("local_to_world", local_to_world) && execution_data.GetValue("world_to_camera", world_to_camera))
			{
				return execution_data.Process(world_to_camera * local_to_world, this);
			}
		}
		// deduced: camera_to_local = inverse(local_to_camera)
		if (auto lock = execution_data.CanDeduce("camera_to_local"))
		{
			glm::mat4 local_to_camera;
			if (execution_data.GetValue("local_to_camera", local_to_camera))
				return execution_data.Process(glm::inverse(local_to_camera), this);

		}
		return GPUProgramProvider::DoProcessAction(execution_data);
	}

}; // namespace chaos
