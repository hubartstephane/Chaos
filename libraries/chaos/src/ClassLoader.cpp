#include <chaos/ClassLoader.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>
#include <chaos/Class.h>

namespace chaos
{
	// an utility function to load a JSON file a find the approriate classname
	template<typename T, typename FUNC>
	static T DoLoadClassHelper(FilePathParam const& path, FUNC func)
	{
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, false))
		{
			std::string class_name;
			if (!JSONTools::GetAttribute(json, "class_name", class_name))
				class_name = BoostTools::PathToName(path.GetResolvedPath());
			if (!class_name.empty())
				return func(class_name.c_str(), json);
		}
		return nullptr;
	}

	Class const * ClassLoader::LoadClass(FilePathParam const& path)
	{
		return DoLoadClassHelper<Class const *>(path, [](char const * class_name, nlohmann::json const & json)
		{
			return Class::DeclareSpecialClass(class_name, json); // a single step operation
		});
	}

	bool ClassLoader::LoadClassesInDirectory(FilePathParam const& path)
	{
		std::vector<Class *> incomplete_classes;

		// try to load all classes : accept incomplete classes
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = chaos::FileTools::GetDirectoryIterator(path); it != end; ++it)
		{
			Class* cls = DoLoadClassHelper<Class *>(it->path(), [] (char const* class_name, nlohmann::json const& json)
			{
				return Class::DoDeclareSpecialClassStep1(class_name, json); // a 2 steps operation 
			});
			// special class MUST have a parent : Finalization to come
			if (cls != nullptr && cls->GetParentClass() == nullptr) 
				incomplete_classes.push_back(cls);
		}
		// complete all classes
		for (Class * cls : incomplete_classes)
			if (!cls->DoDeclareSpecialClassStep2())
				Class::DoInvalidateSpecialClass(cls);
		return true;
	}

}; // namespace chaos
