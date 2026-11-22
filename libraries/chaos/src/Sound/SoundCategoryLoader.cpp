#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	SoundCategory * SoundCategoryLoader::LoadObject(char const * name, nlohmann::json const * json) const
	{
		// no anonymous category
		if (name == nullptr)
			return nullptr;
		// create the object
		SoundCategory * result = manager->AddCategory(name);
		// initialize the new object
		if (result != nullptr)
			result->InitializeFromJSON(json);
		return result;
	}

	bool SoundCategoryLoader::IsNameAlreadyUsedInManager(ObjectRequest request) const
	{
		return (manager->FindCategory(request) != nullptr);
	}

}; // namespace chaos