#include <chaos/Chaos.h>

namespace chaos
{

	//
	// GPUProgramProviderTexture implementation
	//

	bool GPUProgramProviderTexture::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
	{
		if (name != nullptr && handled_name != name)
			return false;
		return action.Process(name, value.get(), this);
	}

	//
	// GPUProgramProviderSearchLock implementation
	//

	GPUProgramProviderSearchLock::GPUProgramProviderSearchLock(GPUProgramProviderSearchLock&& src) :
		searched_name(src.searched_name),
		provider(src.provider)
	{
		src.searched_name = nullptr;
		src.provider = nullptr;
	}

	GPUProgramProviderSearchLock::~GPUProgramProviderSearchLock()
	{
		if (provider != nullptr && searched_name != nullptr)
			provider->pending_searches.erase(std::find(provider->pending_searches.begin(), provider->pending_searches.end(), searched_name));
	}

	GPUProgramProviderSearchLock::operator bool() const
	{
		return (searched_name != nullptr) && (provider != nullptr);
	}

	//
	// GPUProgramProvider implementation
	//

	void GPUProgramProvider::Clear()
	{
		children_providers.clear();
	}

	void GPUProgramProvider::AddVariableProvider(GPUProgramProviderBase * provider)
	{
		if (provider != nullptr)
			children_providers.push_back(provider);
	}

	bool GPUProgramProvider::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
	{
		// handle children providers
		size_t count = children_providers.size();
		for (size_t i = count; i > 0; --i)
		{
			size_t index = i - 1;
			if (children_providers[index]->DoProcessAction(name, action, top_provider))
				return true;
		}
		// failure
		return false;
	}

	GPUProgramProviderSearchLock GPUProgramProvider::DependantSearch(char const* name, char const* searched_name) const
	{
		// not what we are searching ?
		if (StringTools::Strcmp(name, searched_name) != 0)
			return {};
		// already searching this name (avoid infinite recursion)
		if (std::find(pending_searches.begin(), pending_searches.end(), searched_name) != pending_searches.end())
			return {};
		// start a pending request
		pending_searches.push_back(searched_name);
		// return the lock
		GPUProgramProviderSearchLock result;
		result.searched_name = searched_name;
		result.provider = this;
		return std::move(result); // no copy
	}

	//
	// GPUProgramProviderChain implementation
	//

	bool GPUProgramProviderChain::DoProcessAction(char const * name, GPUProgramAction & action, GPUProgramProviderBase const * top_provider) const
	{
		// use variables inside this provider
		if (GPUProgramProvider::DoProcessAction(name, action, top_provider))
			return true;
		// use fallback provider
		if (fallback_provider != nullptr)
			if (fallback_provider->DoProcessAction(name, action, top_provider))
				return true;
		return false;
	}

}; // namespace chaos
