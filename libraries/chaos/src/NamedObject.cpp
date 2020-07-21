#include <chaos/NamedObject.h>
#include <chaos/JSONTools.h>

namespace chaos
{
	void NamedObject::SetName(char const * in_name)
	{
		if (in_name == nullptr)
			name.clear();
		else
			name = in_name;
	}

	void NamedObject::SetObjectNaming(ObjectRequest request)
	{
		if (request.HasStringRequest())
			SetName(request.name);
		if (request.HasTagRequest())
			SetTag(request.tag);	
	}

	bool SaveIntoJSON(nlohmann::json& json_entry, NamedObject const& src)
	{
		if (!json_entry.is_object())
			json_entry = nlohmann::json::object();

		JSONTools::SetAttribute(json_entry, "name", src.GetName());
		JSONTools::SetAttribute(json_entry, "tag", src.GetTag());
		return true;
	}

	bool LoadFromJSON(nlohmann::json const& json_entry, NamedObject& dst)
	{
		if (!json_entry.is_object())
			return false;

		std::string name;
		JSONTools::GetAttribute(json_entry, "name", name, "");
		dst.SetName(name.c_str());

		TagType tag;
		JSONTools::GetAttribute(json_entry, "tag", tag, 0);
		dst.SetTag(tag);
		return true;
	}

}; // namespace chaos
