#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// an utility function to load a JSON file a find the approriate classname
	template<typename T, typename FUNC>
	static T DoLoadClassHelper(FilePathParam const& path, FUNC func)
	{
		nlohmann::json json;
		if (JSONTools::LoadJSONFile(path, json))
		{
			std::string class_name;
			if (!JSONTools::GetAttribute(json, "class_name", class_name))
				class_name = PathTools::PathToName(path.GetResolvedPath());
			if (!class_name.empty())
			{
				std::string short_name;
				JSONTools::GetAttribute(json, "short_name", short_name);
				return func(std::move(class_name), std::move(short_name), json);
			}
		}
		return nullptr;
	}

	Class const * ClassLoader::LoadClass(ClassManager* manager, FilePathParam const& path)
	{
		assert(manager != nullptr);

		return DoLoadClassHelper<Class const *>(path, [this, manager](std::string class_name, std::string short_name, nlohmann::json const & json)
		{
			return DeclareSpecialClass(manager, std::move(class_name), std::move(short_name), json); // a single step operation
		});
	}

	static size_t GetClassDepth(Class const* p)
	{
		size_t result = 0;
		for (; p->GetParentClass() != nullptr; p = p->GetParentClass())
			++result;
		return result;
	}

	bool ClassLoader::LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path)
	{
		assert(manager != nullptr);

		std::vector<Class *> loaded_classes;

		// Step 1: load all classes (no full initialization, ignore parent). Register them (without inheritance data in classes list)
		FileTools::WithDirectoryContent(path, [this, manager, &loaded_classes](boost::filesystem::path const & p)
		{
			Class* cls = DoLoadClassHelper<Class *>(p, [this, manager] (std::string class_name, std::string short_name, nlohmann::json const& json)
			{
				return DoDeclareSpecialClassStep1(manager, std::move(class_name), std::move(short_name), json); // a 3 steps operation
			});
			// remember the class
			if (cls != nullptr)
				loaded_classes.push_back(cls);

			return false; // don't stop
		});

		// Step 2: fix parent classes (all classes coming from json files should have a valid C++ at least or even JSON parent class)
		std::vector<Class*> failing_parent_classes;

		for (Class* cls : loaded_classes)
			if (!DoDeclareSpecialClassStep2(manager, cls))
				failing_parent_classes.push_back(cls);

		// Step 3: find invalid classes and remove them
		if (failing_parent_classes.size() > 0)
		{
			std::vector<Class*> all_failing_classes;
			all_failing_classes.reserve(loaded_classes.size());

			// Step 3.1: construct a vector that owns all failure (direct or indirect)
			for (Class * & cls : loaded_classes)
			{
				for (Class* failing_cls : failing_parent_classes)
				{
					if (cls->InheritsFrom(failing_cls, true) == InheritanceType::YES) // failing class
					{
						Log::Error("Class::LoadClassesInDirectory : special class [%s] as no valid parent.", cls->GetClassName().c_str());
						all_failing_classes.push_back(cls); // this will push classes that fails for parent as well as their children
						cls = nullptr;
						break;
					}
				}
			}

			// Step 3.2: remove all failing class (direct or indirect)
			for (Class* cls : all_failing_classes)
				DoInvalidateSpecialClass(manager, cls);

			// Step 3.3: clean the classes array from nullptr
			auto it = std::ranges::remove_if(loaded_classes, [](Class* cls)
			{
				return (cls == nullptr);
			});
			loaded_classes.erase(it.begin(), it.end());
		}

		// Step 4: sort the classes by depth (use a temp depth map)
		std::map<Class const*, size_t> class_depth;
		for (Class* cls : loaded_classes)
			class_depth[cls] = GetClassDepth(cls);

		std::ranges::sort(loaded_classes, [&class_depth](Class const * c1, Class const * c2)
		{
			return (class_depth[c1] < class_depth[c2]);
		});

		// Step 5: now that we are sorted by depth, we can compute create delegate (create delegate of one Class depends on this parent (lower depth))
		for (Class * cls : loaded_classes)
			if (!DoDeclareSpecialClassStep3(manager, cls))
				DoInvalidateSpecialClass(manager, cls);

		return true;
	}

	Class const* ClassLoader::DeclareSpecialClass(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json const & json)
	{
		Class* result = DoDeclareSpecialClassStep1(manager, std::move(class_name), std::move(short_name), json);
		if (result != nullptr)
		{
			if (!DoDeclareSpecialClassStep2(manager, result) || !DoDeclareSpecialClassStep3(manager, result))
			{
				DoInvalidateSpecialClass(manager, result);
				return nullptr;
			}
		}
		return result;
	}

	Class * ClassLoader::DoDeclareSpecialClassStep1(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json const & json)
	{
		// check parameter and not already registered
		assert(!StringTools::IsEmpty(class_name));
		assert(manager->FindClass(class_name.c_str()) == nullptr);

		if (Class* result = new Class)
		{
			result->name = std::move(class_name);
			result->json_data = json;
			result->declared = true;
			result->manager = manager;
			manager->classes.push_back(result);

			if (!StringTools::IsEmpty(short_name))
				result->SetShortName(std::move(short_name));
			return result;
		}
		return nullptr;
	}

	bool ClassLoader::DoDeclareSpecialClassStep2(ClassManager* manager, Class* cls)
	{
		// parent class is MANDATORY for Special objects
		std::string parent_class_name;
		if (!JSONTools::GetAttribute(cls->json_data, "parent_class", parent_class_name))
		{
			Log::Error("Class::DoDeclareSpecialClassStep2 : special class [%s] require a parent class", cls->name.c_str());
			return false;
		}
		// parent class is MANDATORY for Special objects
		cls->parent = manager->FindClass(parent_class_name.c_str());
		if (cls->parent == nullptr)
		{
			Log::Error("Class::DoDeclareSpecialClassStep2 : special class [%s] has unknown parent class [%s]", cls->name.c_str(), parent_class_name.c_str());
			return false;
		}
		// initialize missing data (size, creation_delegate)
		cls->class_size = cls->parent->class_size;
		return true;
	}

	bool ClassLoader::DoDeclareSpecialClassStep3(ClassManager* manager, Class* cls)
	{
		// check whether it is instanciable
		if (cls->parent->CanCreateInstance())
		{
			cls->create_instance_func = [cls]()
			{
				Object* result = cls->parent->create_instance_func();
				if (result != nullptr)
				{
					JSONSerializable* serializable = auto_cast(result);
					if (serializable != nullptr)
						serializable->SerializeFromJSON(cls->json_data);
				}
				return result;
			};
		}

		// check whether it is temp instanciable
		if (cls->parent->CanCreateInstanceOnStack())
		{
			cls->create_instance_on_stack_func = [cls](std::function<void(Object*)> func)
			{
				cls->parent->create_instance_on_stack_func([cls, func](Object * instance)
				{
					JSONSerializable* serializable = auto_cast(instance);
					if (serializable != nullptr)
						serializable->SerializeFromJSON(cls->json_data);
					func(instance);
				});
			};
		}
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
