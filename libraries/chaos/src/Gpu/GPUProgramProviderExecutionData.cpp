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
		if (StringTools::IsEmpty(searched_name))
			return true;
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

}; // namespace chaos
