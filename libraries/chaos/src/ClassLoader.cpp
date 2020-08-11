#include <chaos/ClassLoader.h>
#include <chaos/BoostTools.h>
#include <chaos/FileTools.h>
#include <chaos/JSONTools.h>
#include <chaos/Class.h>

namespace chaos
{

	Class const * ClassLoader::LoadClass(FilePathParam const& path)
	{
		return DoLoadClass(path, false);
	}

	Class const * ClassLoader::DoLoadClass(FilePathParam const& path, bool accept_unknown_parent)
	{
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json, false))
		{
			std::string class_name;
			if (!JSONTools::GetAttribute(json, "class_name", class_name))
				class_name = BoostTools::PathToName(path.GetResolvedPath());
			if (!class_name.empty())
				return Class::DoDeclareSpecialClass(class_name.c_str(), json, accept_unknown_parent);
		}
		return nullptr;
	}

	bool ClassLoader::LoadClassesInDirectory(FilePathParam const& path)
	{
		std::vector<Class const*> incomplete_classes;

		// try to load all classes : accept incomplete classes
		boost::filesystem::directory_iterator end;
		for (boost::filesystem::directory_iterator it = chaos::FileTools::GetDirectoryIterator(path); it != end; ++it)
		{
			Class const * cls = DoLoadClass(it->path(), true);
			if (cls != nullptr && cls->GetParentClass() == nullptr) // special class MUST have a parent
				incomplete_classes.push_back(cls);
		}

		// complete all classes
		for (Class const* cls : incomplete_classes)
		{
			cls = cls;

		}
		return true;
	}

}; // namespace chaos
