#include <chaos/Chaos.h>

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

	Class const * ClassLoader::LoadClass(FilePathParam const& path)
	{
		return DoLoadClassHelper<Class const *>(path, [this](std::string && class_name, std::string && short_name, nlohmann::json const & json)
		{
			return DeclareSpecialClass(std::move(class_name), std::move(short_name), json); // a single step operation
		});
	}

	bool ClassLoader::LoadClassesInDirectory(FilePathParam const& path)
	{
		std::vector<Class *> classes;

		// Step 1 : load all classes (no full initialization, ignore parent). Register them (without inheritance data in classes list)
		FileTools::ForEachRedirectedDirectoryContent(path, [this, &classes](boost::filesystem::path const & p)
		{
			Class* cls = DoLoadClassHelper<Class *>(p, [this] (std::string && class_name, std::string && short_name, nlohmann::json const& json)
			{
				return DoDeclareSpecialClassStep1(std::move(class_name), std::move(short_name), json); // a 3 steps operation
			});
			// remember the class
			if (cls != nullptr)
				classes.push_back(cls);

			return false; // don't stop
		});

		// Step 2 : fix parent classes
		std::vector<Class*> failing_parent_classes;

		for (Class* cls : classes)
			if (!DoDeclareSpecialClassStep2(cls))
				failing_parent_classes.push_back(cls);

		// Step 3 : find invalid classes and remove them
		if (failing_parent_classes.size() > 0)
		{
			// 2 loops order is important because we may set cls to 0
			for (Class*& cls : classes) // reference so we can invalidate the pointer
			{
				for (Class* failing_cls : failing_parent_classes)
				{
					if (cls->InheritsFrom(failing_cls, true) == InheritanceType::YES) // failing class
					{
						DoInvalidateSpecialClass(cls);
						cls = nullptr;
						break;
					}
				}
			}
			auto it = std::remove(classes.begin(), classes.end(), nullptr);
			classes.erase(it, classes.end());
		}

		// Step 4 : sort the classes by depth (use a temp depth map)
		std::map<Class*, size_t> class_depth;
		for (Class* cls : classes)
			class_depth[cls] = GetDepth(cls);

		std::sort(classes.begin(), classes.end(), [&class_depth](Class * c1, Class * c2)
		{
			return (class_depth[c1] < class_depth[c2]);
		});

		// now that we are sorted by depth, we can compute create delegate (create delegate of one Class depends on this parent (lower depth))
		for (Class * cls : classes)
			if (!DoDeclareSpecialClassStep3(cls))
				DoInvalidateSpecialClass(cls);

		return true;
	}

	size_t ClassLoader::GetDepth(Class const * p) const
	{
		size_t result = 0;
		for (; p->parent != nullptr; p = p->parent)
			++result;
		return result;
	}

	Class const* ClassLoader::DeclareSpecialClass(std::string && class_name, std::string && short_name, nlohmann::json const & json)
	{
		Class* result = DoDeclareSpecialClassStep1(std::move(class_name), std::move(short_name), json);
		if (result != nullptr)
		{
			if (!DoDeclareSpecialClassStep2(result) || !DoDeclareSpecialClassStep3(result))
			{
				DoInvalidateSpecialClass(result);
				return nullptr;
			}
		}
		return result;
	}

	Class * ClassLoader::DoDeclareSpecialClassStep1(std::string && class_name, std::string && short_name, nlohmann::json const & json)
	{
		// check parameter and not already registered
		assert(!StringTools::IsEmpty(class_name));
		assert(Class::FindClass(class_name.c_str()) == nullptr);

		Class* result = new Class;
		if (result != nullptr)
		{
			auto& classes = Class::GetClasses();
			result->name = std::move(class_name);
			result->json_data = json;
			result->declared = true;
			classes[result->name.c_str()] = result; // the key is a pointer aliased on the 'name' member

			if (!StringTools::IsEmpty(short_name))
				result->SetAlias(std::move(short_name));
			return result;
		}
		return nullptr;
	}

	bool ClassLoader::DoDeclareSpecialClassStep2(Class* cls)
	{
		// parent class is MANDATORY for Special objects
		std::string parent_class_name;
		if (!JSONTools::GetAttribute(cls->json_data, "parent_class", parent_class_name))
		{
			Log::Error("Class::DoDeclareSpecialClassStep2 : special class [%s] require a parent class", cls->name.c_str());
			return false;
		}
		// parent class is MANDATORY for Special objects
		cls->parent = Class::FindClass(parent_class_name.c_str());
		if (cls->parent == nullptr)
		{
			Log::Error("Class::DoDeclareSpecialClassStep2 : special class [%s] has unknown parent class [%s]", cls->name.c_str(), parent_class_name.c_str());
			return false;
		}
		// initialize missing data (size, creation_delegate)
		cls->class_size = cls->parent->class_size;
		return true;
	}

	bool ClassLoader::DoDeclareSpecialClassStep3(Class* cls)
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

	void ClassLoader::DoInvalidateSpecialClass(Class const* cls)
	{
		assert(cls != nullptr);
		// remove alias
		if (!StringTools::IsEmpty(cls->alias))
		{
			auto& aliases = Class::GetAliases();
			auto it1 = aliases.lower_bound(cls->alias.c_str());
			auto it2 = aliases.upper_bound(cls->alias.c_str());
			for (auto it = it1; it != it2; ++it)
			{
				if (it->second == cls) // only the alias corresponding to current class
				{
					aliases.erase(it);
					break;
				}
			}
		}
		// remove & delete the class
		auto& classes = Class::GetClasses();
		classes.erase(classes.find(cls->name.c_str()));
		delete(cls);
	}

}; // namespace chaos
