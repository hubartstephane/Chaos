#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/JSONTools.h>
#include <chaos/BoostTools.h>
#include <chaos/FilePath.h>

namespace chaos
{
	// XXX : the path of a resource (texture/program ...) is given by the first time GenTextureObject( PATH ) is called
	//       for example:
	//
	//          GenTextureObject ( PATH = "file1.json" )
	//            -> load JSON file file1.json
	//            -> decrypt PATH = "file2.xxx"
	//               -> GenTextureObject ( PATH = "file2.xxx" )
	//
	//       so, the PATH that is kept is "file1.json" (and not "file2.xxx" even it is the final call)


	/**
	* ResourceManagerLoader
	**/

	template<typename RESOURCE_TYPE, typename PARENT_CLASS, typename MANAGER_TYPE>
	class ResourceManagerLoader : public PARENT_CLASS
	{
	public:

		using resource_type = RESOURCE_TYPE;
		using manager_type = MANAGER_TYPE;
		
		/** constructor */
		ResourceManagerLoader(MANAGER_TYPE * in_manager = nullptr) :
			manager(in_manager)
		{
			
		}

		/** returns the manager */
		manager_type * GetManager() const { return manager; }

		/** set the resolved path (so that it can be given to the result at the end of loading) */
		void SetResultPath(boost::filesystem::path const & path)
		{
			if (!path.empty())
				resolved_path = path;
		}
		/** set the name (so that it can be given to the result at the end of loading) */
		void SetResultName(char const * name)
		{
			if (name != nullptr)
				resource_name = name;
		}

	protected:

		/** search whether the path is already in used in the manager */
		virtual bool IsPathAlreadyUsedInManager(FilePathParam const & path) const { return false; }
		/** search whether the name is already in used in the manager */
		virtual bool IsNameAlreadyUsedInManager(char const * name) const { return false; }

		/** set the path of currently loaded resource if not already set, and if no collision detected */
		bool CheckResourcePath(FilePathParam const & path) const
		{
			// path already known, nothing to do
			if (!resolved_path.empty())
				return true;
			// path already exising in manager : failure
			if (manager != nullptr && IsPathAlreadyUsedInManager(path))
				return false;
			// the currently loaded resource has now a path
			resolved_path = path.GetResolvedPath();
			return true;
		}
		/** set the name of currently loaded resource if not already set, and if no collision detected */
		bool CheckResourceName(boost::filesystem::path const * in_path, char const * in_name, nlohmann::json const * json) const
		{
			// name already known, nothing to do
			if (!resource_name.empty())
				return true;
			// compute the name we want from different entries (reuse in_name if null)
			std::string tmp_name;
			if (in_name == nullptr)
			{
				if (in_path != nullptr)
					tmp_name = BoostTools::PathToName(*in_path).c_str();
				else if (json != nullptr)
					JSONTools::GetAttribute(*json, "name", tmp_name);
				if (tmp_name.empty()) // still no name, can not continue
					return false;
				in_name = tmp_name.c_str();
			}
			// name already exising in manager : failure
			if (manager != nullptr && IsNameAlreadyUsedInManager(in_name))
				return false;
			// the currently loaded resource has now a name
			if (!tmp_name.empty())
				resource_name = std::move(tmp_name); // can steal resource
			else
				resource_name = in_name; // have to make a copy
			return true;
		}
		/** apply the name to resource */
		void ApplyNameToLoadedResource(RESOURCE_TYPE * resource) const
		{
			if (resource == nullptr)
				return;			
			// should we update the path of the object ?
			if (resource_name.empty())
				return;
			// apply the name
			char const * name = resource->GetName();
			if (name == nullptr || name[0] == 0)
				SetResourceName(resource, resource_name.c_str());
		}
		/** apply the path to resource */
		void ApplyPathToLoadedResource(RESOURCE_TYPE * resource) const
		{
			if (resource == nullptr)
				return;
			// should we update the path of the object ?
			if (resolved_path.empty())
				return;
			// should we update the path of the object ?
			if (resource->GetPath().empty())
				SetResourcePath(resource, resolved_path);
		}
		/** apply name/path to resource then clean loader */
		void FinalizeLoadedResource(RESOURCE_TYPE * resource) const
		{
			ApplyNameToLoadedResource(resource);
			ApplyPathToLoadedResource(resource);
			resource_name = std::string();
			resolved_path = boost::filesystem::path(); // ready for next call
		}

	protected:

		/** the resource manager of interest */
		MANAGER_TYPE * manager = nullptr;
		/** the name of currently loaded resource (the very first name encoutered in loading call chain is the good) */
		mutable std::string resource_name;
		/** the path of currently loaded resource (the very first path encoutered in loading call chain is the good) */
		mutable boost::filesystem::path resolved_path;
	};

}; // namespace chaos
