#include <chaos/Chaos.h>

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
		std::vector<Class *> classes;

		// Step 1 : load all classes (no full initialization, ignore parent). Register them (without inheritance data in classes list)
		FileTools::ForEachRedirectedDirectoryContent(path, [this, &classes](boost::filesystem::path const & p) 
		{
			Class* cls = DoLoadClassHelper<Class *>(p, [] (char const* class_name, nlohmann::json const& json)
			{
				return Class::DoDeclareSpecialClassStep1(class_name, json); // a 2 steps operation 
			});
			// remember the class
			if (cls != nullptr)
				classes.push_back(cls);		
		
			return false; // don't stop
		});

		// Step 2 : fix parent classes
		std::vector<Class*> failing_parent_classes;

		for (Class* cls : classes)
			if (!cls->DoDeclareSpecialClassStep2())
				failing_parent_classes.push_back(cls);

		// Step 3 : find invalid classes
		std::vector<Class*> to_remove_classes;

		for (Class* cls : classes)
			for (Class* failing_cls : failing_parent_classes)
				if (cls->InheritsFrom(failing_cls, true) == InheritanceType::YES) // failing class goes directly in the to_remove vector too 
					to_remove_classes.push_back(cls);
		// Step 4 : remove classes
		for (Class* cls : to_remove_classes)
		{
			classes.erase(std::remove(classes.begin(), classes.end(), cls)); // just keep in that array the classes that are fully valid
			Class::DoInvalidateSpecialClass(cls);
		}
		// Step 5 : sort the classes by depth (use a temp depth map)
		std::map<Class*, size_t> class_depth;
		for (Class* cls : classes)
			class_depth[cls] = cls->GetDepth();

		std::sort(classes.begin(), classes.end(), [&class_depth](Class * c1, Class * c2)
		{
			return (class_depth[c1] < class_depth[c2]);
		});
			
		// now that we are sorted by depth, we can compute create delegate (create delegate of one Class depends on this parent (lower depth))
		for (Class * cls : classes)
			if (!cls->DoDeclareSpecialClassStep3())		
				Class::DoInvalidateSpecialClass(cls);
		
		return true;
	}

}; // namespace chaos
