#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool PersistentDataInterface::ReadPersistentData()
	{
		if (nlohmann::json const* json = GetPersistentReadStorage())
		{
			OnReadPersistentData(json);
			return true;
		}
		return false;
	}

	bool PersistentDataInterface::WritePersistentData() const
	{
		if (nlohmann::json * json = GetPersistentWriteStorage())
		{
			OnWritePersistentData(json);
			return true;
		}
		return false;
	}

}; // namespace chaos