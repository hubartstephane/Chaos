#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	
	bool ResourceManagerLoaderBase::CheckResourcePath(FilePathParam const& path) const
	{
		// path already known, nothing to do
		if (!resolved_path.empty())
			return true;
		// path already existing in manager : failure
		if (IsPathAlreadyUsedInManager(path))
			return false;
		// the currently loaded resource has now a path
		resolved_path = path.GetResolvedPath();
		return true;
	}
	
	bool ResourceManagerLoaderBase::CheckResourceName(boost::filesystem::path const* in_path, char const* in_name, nlohmann::json const* json) const
	{
		// name already known, nothing to do
		if (!resource_name.empty())
			return true;
		// compute the name we want from different entries (reuse in_name if null)
		std::string tmp_name;
		if (in_name == nullptr)
		{
			if (in_path != nullptr)
				tmp_name = PathTools::PathToName(*in_path).c_str();
			else if (json != nullptr)
				JSONTools::GetAttribute(json, "name", tmp_name);

			// XXX : to test without 
			//       this line of code prevent to have anymous object that can be declared inline
			//       for example, a program discribed inplace by {...}
			//       inside the material itself

			// if (tmp_name.empty()) // still no name, can not continue
			//	return false;

			in_name = tmp_name.c_str();
		}
		// name already exising in manager (or empty -> cannot insert anonymous object in manager): failure
		if (IsNameAlreadyUsedInManager(in_name))
			return false;
		// the currently loaded resource has now a name
		if (!tmp_name.empty())
			resource_name = std::move(tmp_name); // can steal resource
		else
			resource_name = in_name; // have to make a copy
		return true;
	}
	
	void ResourceManagerLoaderBase::ApplyNameToLoadedResource(NamedInterface* resource) const
	{
		if (resource != nullptr)
		{
			if (!resource_name.empty())
			{
				char const* name = resource->GetName();
				if (StringTools::IsEmpty(name))
					resource->SetName(resource_name.c_str());
			}
		}
		resource_name = std::string(); // reset
	}
	
	void ResourceManagerLoaderBase::ApplyPathToLoadedResource(FileResource* resource) const
	{
		if (resource != nullptr)
		{
			if (!resolved_path.empty())
			{
				if (resource->GetPath().empty())
					resource->SetPath(resolved_path);
			}
		}
		resolved_path = boost::filesystem::path(); // reset
	}

}; // namespace chaos
