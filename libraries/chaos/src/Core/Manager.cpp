#include <chaos/Chaos.h>

namespace chaos
{

	bool Manager::StartManager()
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

	bool Manager::StopManager()
	{
		if (!IsManagerStarted())
			return false;
		DoStopManager();
		manager_started = true;
		return true;
	}

	bool Manager::DoStartManager()
	{
		return true;
	}

	bool Manager::DoStopManager()
	{
		return true;
	}

	bool Manager::IsManagerStarted() const
	{
		return manager_started;
	}

	bool Manager::InitializeFromConfiguration(JSONConfig const & config)
	{
		return true;
	}

	bool Manager::CanAddObject(ObjectRequest request, std::function<bool(ObjectRequest)> can_add_func) const
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