#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// an utility function to load a JSON file a find the approriate classname
	template<typename FUNC>
	static Class * DoLoadClassHelper(FilePathParam const& path, FUNC func)
	{
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json))
		{
			// get or build a class name
			std::string class_name;
			if (!JSONTools::GetAttribute(json, "class_name", class_name))
				class_name = PathTools::PathToName(path.GetResolvedPath());
			// parent class is MANDATORY for Special objects
			std::string parent_class_name;
			if (!JSONTools::GetAttribute(json, "parent_class", parent_class_name))
			{
				Log::Error("DoLoadClassHelper : special class [%s] require a parent class", class_name.c_str());
				return nullptr;
			}
			// create the class
			if (!class_name.empty())
			{
				std::string short_name;
				JSONTools::GetAttribute(json, "short_name", short_name);
				return func(std::move(class_name), std::move(short_name), std::move(parent_class_name), std::move(json));
			}
		}
		return nullptr;
	}

	Class * ClassLoader::LoadClass(ClassManager* manager, FilePathParam const& path)
	{
		assert(manager != nullptr);

		return DoLoadClassHelper(path, [this, manager](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
		{
			return DeclareSpecialClass(manager, std::move(class_name), std::move(short_name), parent_class_name, std::move(json)); // a single step operation
		});
	}

	bool ClassLoader::LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path)
	{
		assert(manager != nullptr);

		using class_registration_type = std::pair<Class*, std::string>; // <class, parent_class_name>

		std::vector<class_registration_type> loaded_classes;

		// Step 1: load all classes (no full initialization, ignore parent). Register them (without inheritance data in classes list)
		FileTools::WithDirectoryContent(path, [this, manager, &loaded_classes](boost::filesystem::path const & p)
		{
			DoLoadClassHelper(p, [this, manager, &loaded_classes] (std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
			{
				Class* result = DoDeclareSpecialClassCreate(manager, std::move(class_name), std::move(short_name), std::move(json));
				if (result != nullptr)
					loaded_classes.push_back({ result, std::move(parent_class_name) });
				return result;
			});
			return false; // don't stop
		});

		// Step 2: fix parent classes (all classes coming from json files should have a valid C++ at least or even JSON parent class)
		std::vector<Class*> failing_parent_classes;

		for (class_registration_type & class_registration : loaded_classes)
			if (!DoDeclareSpecialClassPatchParent(manager, class_registration.first, class_registration.second))
				failing_parent_classes.push_back(class_registration.first);

		// Step 3: find invalid classes and remove them
		if (failing_parent_classes.size() > 0)
		{
			std::vector<Class*> all_failing_classes;
			all_failing_classes.reserve(loaded_classes.size());

			// Step 3.1: construct a vector that owns all failures (direct or indirect)
			for (class_registration_type const & class_registration : loaded_classes)
			{
				for (Class* failing_cls : failing_parent_classes)
				{
					if (class_registration.first->InheritsFrom(failing_cls, true) == InheritanceType::YES) // failing class
					{
						Log::Error("Class::LoadClassesInDirectory : special class [%s] has no valid parent.", class_registration.first->GetClassName().c_str());
						all_failing_classes.push_back(class_registration.first); // this will push classes that fails for parent as well as their children
						break;
					}
				}
			}

			// Step 3.2: remove all failing class (direct or indirect)
			for (Class* cls : all_failing_classes)
				DoInvalidateSpecialClass(manager, cls);
		}
		return true;
	}

	Class * ClassLoader::DeclareSpecialClass(ClassManager* manager, std::string class_name, std::string short_name, std::string const &parent_class_name, nlohmann::json json)
	{
		Class* result = DoDeclareSpecialClassCreate(manager, std::move(class_name), std::move(short_name), std::move(json));
		if (result != nullptr)
		{
			if (!DoDeclareSpecialClassPatchParent(manager, result, parent_class_name))
			{
				DoInvalidateSpecialClass(manager, result);
				return nullptr;
			}
		}
		return result;
	}

	Class * ClassLoader::DoDeclareSpecialClassCreate(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json)
	{
		// check parameter and not already registered
		assert(!StringTools::IsEmpty(class_name));
		assert(manager->FindClass(class_name.c_str()) == nullptr);

		if (Class* result = manager->CreateClass(std::move(class_name)))
		{
			result->declared = true;
			if (!StringTools::IsEmpty(short_name))
				result->SetShortName(std::move(short_name));
			result->AddObjectInitializationFunction([json = std::move(json)](Object* object)
			{
				if (JSONSerializable* serializable = auto_cast(object))
					serializable->SerializeFromJSON(json);
			});
			return result;
		}
		return nullptr;
	}

	bool ClassLoader::DoDeclareSpecialClassPatchParent(ClassManager* manager, Class* cls, std::string const & parent_class_name)
	{
		// parent class is MANDATORY for Special objects
		cls->parent = manager->FindClass(parent_class_name.c_str());
		if (cls->parent == nullptr)
		{
			Log::Error("Class::DoDeclareSpecialClassPatchParent : special class [%s] has unknown parent class [%s]", cls->name.c_str(), parent_class_name.c_str());
			return false;
		}
		// initialize missing data (size, creation_delegate)
		cls->class_size = cls->parent->class_size;
		return true;
	}

	void ClassLoader::DoInvalidateSpecialClass(ClassManager* manager, Class const* cls)
	{
		assert(cls != nullptr);

		// remove & delete the class
		auto& classes = manager->classes;
		classes.erase(std::ranges::find(classes, cls));
		delete(cls);
	}

}; // namespace chaos
