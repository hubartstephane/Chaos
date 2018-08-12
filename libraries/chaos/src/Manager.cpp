#include <chaos/Manager.h>
#include <chaos/JSONTools.h>

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

	bool Manager::InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path)
	{
		return true;
	}

}; // namespace chaos