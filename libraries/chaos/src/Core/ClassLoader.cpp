#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// an utility function to load a JSON file a find the approriate classname
	static Class* DoLoadClassHelper(FilePathParam const& path, LightweightFunction<Class* (std::string, std::string, std::string, nlohmann::json)> func)
	{
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json))
		{
			// get or build a class name
			std::string class_name;
			if (!JSONTools::GetAttribute(&json, "class_name", class_name))
				class_name = PathTools::PathToName(path.GetResolvedPath());
			// parent class is MANDATORY for Special objects
			std::string parent_class_name;
			if (!JSONTools::GetAttribute(&json, "parent_class", parent_class_name))
			{
				ClassLog::Error("DoLoadClassHelper : special class [%s] require a parent class", class_name.c_str());
				return nullptr;
			}
			// create the class
			if (!class_name.empty())
			{
				std::string short_name;
				JSONTools::GetAttribute(&json, "short_name", short_name);
				return func(std::move(class_name), std::move(short_name), std::move(parent_class_name), std::move(json));
			}
		}
		return nullptr;
	}

	Class * ClassLoader::LoadClass(ClassManager* manager, FilePathParam const& path) const
	{
		assert(manager != nullptr);

		return DoLoadClassHelper(path, [this, manager](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json) -> Class *
		{
			if (Class* result = DoCreateSpecialClass(manager, std::move(class_name), std::move(short_name), std::move(json)))
			{
				if (DoSetSpecialClassParent(manager, result, parent_class_name))
				{
					DoCompleteSpecialClassMissingData(result);
					return result;
				}
				DoDeleteSpecialClass(manager, result);
			}
			return nullptr;
		});
	}

	bool ClassLoader::LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path) const
	{
		assert(manager != nullptr);

		using ClassRegistrationType = std::pair<Class*, std::string>; // <class, parent_class_name>

		std::vector<ClassRegistrationType> loaded_classes;

		// Step 1: load all classes (no full initialization, ignore parent). Register them (without inheritance data in classes list)
		FileTools::WithDirectoryContent(path, [this, manager, &loaded_classes](boost::filesystem::path const & p)
		{
			DoLoadClassHelper(p, [this, manager, &loaded_classes] (std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
			{
				Class* result = DoCreateSpecialClass(manager, std::move(class_name), std::move(short_name), std::move(json));
				if (result != nullptr)
					loaded_classes.push_back({ result, std::move(parent_class_name) });
				return result;
			});
			return false; // don't stop
		});

		// Step 2: set parents
		for (ClassRegistrationType& class_registration : loaded_classes)
			DoSetSpecialClassParent(manager, class_registration.first, class_registration.second);

		//  Step 3: complete inheritance chain
		std::vector<Class*> to_remove;

		for (ClassRegistrationType& class_registration : loaded_classes)
		{
			if (!DoCompleteSpecialClassMissingData(class_registration.first)) // if it fais, this means that parent chain is broken
			{
				ClassLog::Error("Class::LoadClassesInDirectory : special class [%s] has a broken inheritance chain.", class_registration.first->GetClassName().c_str());
				to_remove.push_back(class_registration.first);
			}
		}

		// Step 4: remove classes that have to
		for (Class* cls : to_remove)
			DoDeleteSpecialClass(manager, cls);
		return true;
	}

	bool ClassLoader::DoCompleteSpecialClassMissingData(Class* cls) const
	{
		// the class is already fully initialized
		if (cls->declared)
			return true;
		// cannot complete the initialization
		if (cls->parent == nullptr)
			return false;
		// recursively initialize children
		if (!DoCompleteSpecialClassMissingData(const_cast<Class*>(cls->parent))) // forced to remove constness here
			return false;
		// get missing data
		cls->declared = true;
		cls->class_size = cls->parent->class_size;
		cls->info = cls->parent->info;
		return true;
	}

	Class * ClassLoader::DoCreateSpecialClass(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const
	{
		// check parameter and not already registered
		assert(!StringTools::IsEmpty(class_name));

		if (manager->FindClass(class_name.c_str(), FindClassFlags::NAME) != nullptr)
		{
			ClassLog::Error("ClassLoader::DoCreateSpecialClass(...): class already existing [%s]", class_name.c_str());
			return nullptr;
		}

		if (Class* result = new ClassWithJSONInitialization(std::move(class_name), std::move(json)))
		{
			if (!StringTools::IsEmpty(short_name))
				result->SetShortName(std::move(short_name));
			manager->InsertClass(result);
			return result;
		}
		return nullptr;
	}

	bool ClassLoader::DoSetSpecialClassParent(ClassManager* manager, Class* cls, std::string const & parent_class_name) const
	{
		// parent class is MANDATORY for Special objects
		cls->parent = manager->FindClass(parent_class_name.c_str());
		if (cls->parent == nullptr)
		{
			ClassLog::Error("Class::DoSetSpecialClassParent : special class [%s] has unknown parent class [%s]", cls->name.c_str(), parent_class_name.c_str());
			return false;
		}
		return true;
	}

	void ClassLoader::DoDeleteSpecialClass(ClassManager* manager, Class * cls) const
	{
		// remove & delete the class
		assert(cls != nullptr);
		manager->classes.erase(std::ranges::find(manager->classes, cls));
		delete(cls);
	}

	ClassWithJSONInitialization::ClassWithJSONInitialization(std::string in_name, nlohmann::json in_json) :
		Class(std::move(in_name)),
		json(std::move(in_json))
	{
	}

	void ClassWithJSONInitialization::OnObjectInstanceInitialized(Object* object) const
	{
		if (JSONSerializableInterface* serializable = auto_cast(object))
			serializable->SerializeFromJSON(&json);
	}

}; // namespace chaos
