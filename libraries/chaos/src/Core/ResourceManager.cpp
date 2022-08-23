#include "chaos/ChaosPCH.h"
#include "chaos/Chaos.h"

namespace chaos
{

	bool ResourceManager::StartManager()
	{
		if (IsManagerStarted())
			return true;
		if (!DoStartManager())
		{
			DoStopManager();
			return false;
		}
		manager_started = true;
		return true;
	}

	bool ResourceManager::StopManager()
	{
		if (!IsManagerStarted())
			return false;
		DoStopManager();
		manager_started = true;
		return true;
	}

	bool ResourceManager::DoStartManager()
	{
		return true;
	}

	bool ResourceManager::DoStopManager()
	{
		return true;
	}

	bool ResourceManager::IsManagerStarted() const
	{
		return manager_started;
	}

	bool ResourceManager::InitializeFromConfiguration(nlohmann::json const & config)
	{
		return true;
	}

	bool ResourceManager::CanAddObject(ObjectRequest request, std::function<bool(ObjectRequest)> can_add_func) const
	{
		// manager initialized ?
		if (!IsManagerStarted())
			return false;
		// name already existing ?
		if (!request.IsNoneRequest() && !can_add_func(request))
			return false;
		return true;
	}

}; // namespace chaos