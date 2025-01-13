#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

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
			if (entry.process_func) // can be a function/lambda
			{
				if (entry.process_func(execution_data))
					return true;
			}
			else if (entry.provider != nullptr) // or can be another provider
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

		if (execution_data.GetPassType() == GPUProgramProviderPassType::FALLBACK)
		{
			for (char const* name : {
				"local_to_world",
				"world_to_local",
				"world_to_camera",
				"camera_to_world",
				"local_to_camera",
				"camera_to_local",
				"local_to_world_normal",
				"world_to_local_normal",
				"world_to_camera_normal",
				"camera_to_world_normal"
				"local_to_camera_normal",
				"camera_to_local_normal"
				})
			{
				static const glm::mat4 identity = glm::scale(glm::vec3(1.0f, 1.0f, 1.0f));

				if (execution_data.Match(name, GPUProgramProviderPassType::FALLBACK))
					return execution_data.Process(identity);
			}
		}

		// deduce from inverse matrix
		auto DeduceFromInverseMatrix = [this, &execution_data](char const* target_matrix_name, char const* source_matrix_name)
		{
			if (auto lock = execution_data.CanDeduce(target_matrix_name))
			{
				glm::mat4 transform;
				if (execution_data.GetValue(source_matrix_name, transform))
					return execution_data.Process(glm::inverse(transform));
			}
			return false;
		};

		if (DeduceFromInverseMatrix("local_to_world", "world_to_local"))
			return true;
		if (DeduceFromInverseMatrix("world_to_local", "local_to_world"))
			return true;
		if (DeduceFromInverseMatrix("camera_to_world", "world_to_camera"))
			return true;
		if (DeduceFromInverseMatrix("world_to_camera", "camera_to_world"))
			return true;
		if (DeduceFromInverseMatrix("local_to_camera", "camera_to_local"))
			return true;
		if (DeduceFromInverseMatrix("camera_to_local", "local_to_camera"))
			return true;

		// deduce from products
		auto DeduceFromMatrixComposition = [this, &execution_data](char const* target_matrix_name, char const* source_matrix_name1, char const* source_matrix_name2)
		{
			if (auto lock = execution_data.CanDeduce(target_matrix_name))
			{
				glm::mat4 m1;
				glm::mat4 m2;
				if (execution_data.GetValue(source_matrix_name1, m1) && execution_data.GetValue(source_matrix_name2, m2))
					return execution_data.Process(m2 * m1);
			}
			return false;
		};

		if (DeduceFromMatrixComposition("local_to_camera", "local_to_world", "world_to_camera"))
			return true;
		if (DeduceFromMatrixComposition("camera_to_local", "camera_to_world", "world_to_local"))
			return true;

		// deduce normal transform matrix
		auto DeduceNormalTransformMatrix = [this, &execution_data](char const* target_matrix_name, char const* source_matrix_name)
		{
			if (auto lock1 = execution_data.CanDeduce(target_matrix_name))
			{
				glm::mat4 m;
				if (execution_data.GetValue(source_matrix_name, m))
					return execution_data.Process(glm::inverse(glm::transpose(m)));
			}
			return false;
		};

		if (DeduceNormalTransformMatrix("local_to_world_normal", "local_to_world"))
			return true;
		if (DeduceNormalTransformMatrix("world_to_local_normal", "world_to_local"))
			return true;
		if (DeduceNormalTransformMatrix("camera_to_world_normal", "camera_to_world"))
			return true;
		if (DeduceNormalTransformMatrix("world_to_camera_normal", "world_to_camera"))
			return true;
		if (DeduceNormalTransformMatrix("local_to_camera_normal", "local_to_camera"))
			return true;
		if (DeduceNormalTransformMatrix("camera_to_local_normal", "camera_to_local"))
			return true;

		return GPUProgramProvider::DoProcessAction(execution_data);
	}

}; // namespace chaos
