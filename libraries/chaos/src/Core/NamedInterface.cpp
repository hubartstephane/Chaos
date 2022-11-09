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

	bool SaveIntoJSON(nlohmann::json& json, NamedInterface const& src)
	{
		if (!json.is_object())
			json = nlohmann::json::object();

		JSONTools::SetAttribute(json, "name", src.GetName());
		JSONTools::SetAttribute(json, "tag", src.GetTag());
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json, NamedInterface& dst)
	{
		if (!json.is_object())
			return false;

		std::string name;
		JSONTools::GetAttribute(json, "name", name, "");
		dst.SetName(name.c_str());

		TagType tag;
		JSONTools::GetAttribute(json, "tag", tag, 0);
		dst.SetTag(tag);
		return true;
	}

}; // namespace chaos
