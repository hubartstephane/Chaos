#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	void NamedInterface::SetName(char const * in_name)
	{
		if (in_name == nullptr)
			name.clear();
		else
			name = in_name;
	}

	void NamedInterface::SetObjectNaming(ObjectRequest request)
	{
		if (request.IsStringRequest())
			SetName(request.name);
		else if (request.IsTagRequest())
			SetTag(request.tag);
	}

	bool DoSaveIntoJSON(nlohmann::json * json, NamedInterface const& src)
	{
		if (!PrepareSaveIntoJSON(json))
			return false;
		JSONTools::SetAttribute(json, "name", src.GetName());
		JSONTools::SetAttribute(json, "tag", src.GetTag());
		return true;
	}

	bool DoLoadFromJSON(JSONReadConfiguration config, NamedInterface& dst)
	{
		std::string name;
		JSONTools::GetAttribute(config, "name", name, "");
		dst.SetName(name.c_str());

		TagType tag;
		JSONTools::GetAttribute(config, "tag", tag, 0);
		dst.SetTag(tag);
		return true;
	}

}; // namespace chaos
