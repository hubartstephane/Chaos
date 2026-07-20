#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	bool ClassLoader::DoLoadClassHelper(FilePathParam const& path, LightweightFunction<bool(std::string, std::string, std::string, nlohmann::json)> func) const
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
				return false;
			}
			// create the class
			if (!class_name.empty())
			{
				std::string short_name;
				JSONTools::GetAttribute(&json, "short_name", short_name);
				return func(std::move(class_name), std::move(short_name), std::move(parent_class_name), std::move(json));
			}
		}
		return false;
	}

	Class<Object>* ClassLoader::LoadClass(ClassManager* manager, FilePathParam const& path) const
	{
		assert(manager != nullptr);

		Class<Object>* result = nullptr;
		DoLoadClassHelper(path, [this, manager, &result](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
		{
			result = DoCreateChildClass(manager, std::move(class_name), std::move(short_name), std::move(json), parent_class_name.c_str());
			return (result != nullptr);
		});
		return result;
	}

	Class<Object>* ClassLoader::DoCreateChildClass(ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json, char const* parent_class_name) const
	{
		// check parameter and not already registered
		assert(!StringTools::IsEmpty(class_name));

		if (manager->FindClass(class_name.c_str(), FindClassFlags::Name) != nullptr)
		{
			ClassLog::Error("ClassLoader::DoCreateChildClass(...): class already existing [%s]", class_name.c_str());
			return nullptr;
		}

		Class<Object>* parent_class = manager->FindClass(parent_class_name);
		if (parent_class == nullptr)
		{
			ClassLog::Error("ClassLoader::DoCreateChildClass(...): can't find parent class [%s] derived from Object", parent_class_name);
			return nullptr;
		}

		return DoCreateChildClassHelper
		(
			parent_class,
			manager,
			std::move(class_name),
			std::move(short_name),
			std::move(json)
		);
	}

	Class<Object>* ClassLoader::DoCreateChildClassHelper(Class<Object>* parent_class, ClassManager* manager, std::string class_name, std::string short_name, nlohmann::json json) const
	{
		Class<Object>* result = parent_class->CreateSubclass(manager, std::move(class_name), [json = std::move(json)](Object* object)
		{
			if (JSONSerializableInterface* serializable = auto_cast(object))
				serializable->SerializeFromJSON(&json);
		});

		if (result == nullptr)
			return nullptr;

		if (!StringTools::IsEmpty(short_name))
			result->SetShortName(std::move(short_name));
		manager->InsertClass(result);

		return result;
	}

	struct LoadingClassInfo
	{
		/** the name of the class to create */
		std::string class_name;
		/** the shortname of the class to create */
		std::string short_name;
		/** the name of the parent_class to create */
		std::string parent_class_name;
		/** the json data to be used to initialize new instances */
		nlohmann::json json;

		/** the parent class_info */
		LoadingClassInfo* parent_class_info = nullptr;
		/** the parent class */
		Class<Object>* parent_class = nullptr;
		/** the effective class created */
		Class<Object>* loaded_class = nullptr;

		/** the level in hierarchy of class. Higher values must be treated first */
		int  level = 0;
		/** whether this info is valid, or is to be rejected */
		bool is_valid = true;
		/** used to detect circular classes hierarchy */
		bool traversal_in_progress = false;
	};

	void SearchCircularReferenceMarkInvalid(LoadingClassInfo* cls, bool& error_circular_reference)
	{
		if (cls == nullptr)
			return;
		else if (!cls->is_valid)
			return;
		else if (cls->traversal_in_progress)
		{
			cls->is_valid = false;
			error_circular_reference = true;
		}
		else
		{
			cls->traversal_in_progress = true;
			SearchCircularReferenceMarkInvalid(cls->parent_class_info, error_circular_reference);
			cls->traversal_in_progress = false;
		}
	}

	bool ClassLoader::LoadClassesInDirectory(ClassManager* manager, FilePathParam const& path) const
	{
		assert(manager != nullptr);

		std::vector<LoadingClassInfo> loaded_classes_info;

		FileTools::WithDirectoryContent(path, [this, &loaded_classes_info](boost::filesystem::path const& p)
		{
			DoLoadClassHelper(p, [this, &loaded_classes_info](std::string class_name, std::string short_name, std::string parent_class_name, nlohmann::json json)
			{
				LoadingClassInfo class_info;
				class_info.class_name = std::move(class_name);
				class_info.short_name = std::move(short_name);
				class_info.parent_class_name = std::move(parent_class_name);
				class_info.json = std::move(json);
				loaded_classes_info.push_back(std::move(class_info));
				return true;
			});
			return false; // do not stop iterating in directory
		});

		if (loaded_classes_info.size() == 0)
			return true;

		// generate indices
		std::vector<size_t> indices;
		indices.reserve(loaded_classes_info.size());
		for (size_t i = 0; i < loaded_classes_info.size(); ++i)
			indices.push_back(i);

		auto RemoveInvalidClasses = [&indices, &loaded_classes_info](LightweightFunction<void(LoadingClassInfo const& class_info)> log_func)
		{
			std::erase_if(indices, [&loaded_classes_info, &log_func](size_t index)
			{
				if (!loaded_classes_info[index].is_valid)
				{
					log_func(loaded_classes_info[index]);
					return true;
				}
				return false;
			});
		};

		// search classes already existing in manager
		bool error_already_existing = false;
		for (size_t i = 0; i < indices.size(); ++i)
		{
			LoadingClassInfo& class_info = loaded_classes_info[indices[i]];

			if (manager->FindClass(class_info.class_name.c_str(), FindClassFlags::Name) != nullptr) // ignore short name and parent manager
			{
				class_info.is_valid = false;
				error_already_existing = true;
			}
		}

		if (error_already_existing)
		{
			RemoveInvalidClasses([](LoadingClassInfo const& class_info)
			{
				ClassLog::Error("Loading class [%s] failure: already exists in manager", class_info.class_name.c_str());
			});
		}

		// link child-parent alltogether
		for (size_t index1 : indices)
		{
			LoadingClassInfo& class_info1 = loaded_classes_info[index1];
			for (size_t index2 : indices)
			{
				if (index1 == index2)
					continue;
				LoadingClassInfo& class_info2 = loaded_classes_info[index2];
				if (class_info1.parent_class_name == class_info2.class_name)
				{
					class_info1.parent_class_info = &class_info2;
					break;
				}
			}
		}

		// search real base class from manager
		bool error_base_class_not_found = false;
		for (size_t index : indices)
		{
			LoadingClassInfo& class_info = loaded_classes_info[index];

			if (class_info.parent_class_info != nullptr) // we need to find an already exising base class ?
				continue;
			if (Class<Object>* parent_class = manager->FindClass(class_info.parent_class_name.c_str()))
			{
				class_info.parent_class = parent_class;
			}
			else
			{
				class_info.is_valid = false;
				error_base_class_not_found = true;
			}
		}

		if (error_base_class_not_found)
		{
			RemoveInvalidClasses([](LoadingClassInfo const& class_info)
			{
				ClassLog::Error("Loading class [%s] failure: parent [%s] not found", class_info.class_name.c_str(), class_info.parent_class_name.c_str());
			});
		}

		// eliminate circular referenced classes
		bool error_circular_reference = false;
		for (size_t index : indices)
		{
			LoadingClassInfo& class_info = loaded_classes_info[index];
			SearchCircularReferenceMarkInvalid(&class_info, error_circular_reference);
		}

		if (error_circular_reference)
		{
			RemoveInvalidClasses([](LoadingClassInfo const& class_info)
			{
				ClassLog::Error("Loading class [%s] failure: circular reference", class_info.class_name.c_str());
			});
		}

		// propagate invalidity
		bool error_propagation = false;

		do
		{
			error_propagation = false;

			for (size_t index : indices)
			{
				LoadingClassInfo& class_info = loaded_classes_info[index];
				if (class_info.parent_class_info != nullptr && !class_info.parent_class_info->is_valid)
				{
					class_info.is_valid = false;
					error_propagation = true;
				}
			}
			if (error_propagation)
			{
				RemoveInvalidClasses([](LoadingClassInfo const& class_info)
				{
					ClassLog::Error("Loading class [%s] failure: invalid parent [%s]", class_info.class_name.c_str(), class_info.parent_class_name.c_str());
				});
			}
		} while (error_propagation);

		// 'sort' classes
		// in a class is a 'parent/root' it has higher level value
		// because the node is traversed more times
		// 
		//  B inherits of A
		//
		//     A     level = 2 (because there are two traversals of this class. one starting from A, one starting from B
		//     |
		//     v
		//     B     level = 1 (a single traversal for this class. the one starting from B)
		//
		// at the end, we just have to handle classes in order of decreasing values
		auto IncrementClassDataLevel = [](LoadingClassInfo* class_info)
		{
			do
			{
				++class_info->level;
				class_info = class_info->parent_class_info;
			} while (class_info != nullptr);
		};

		for (size_t index : indices)
			IncrementClassDataLevel(&loaded_classes_info[index]);

		// sort thoses indices according the level
		std::ranges::sort(indices, [&loaded_classes_info](size_t index1, size_t index2)
			{
				return loaded_classes_info[index1].level > loaded_classes_info[index2].level;
			});

		// handle class_info according to the decreasing level
		for (size_t index : indices)
		{
			LoadingClassInfo& class_info = loaded_classes_info[index];

			if (class_info.parent_class != nullptr)
			{
				class_info.loaded_class = DoCreateChildClassHelper(
					class_info.parent_class,
					manager,
					std::move(class_info.class_name),
					std::move(class_info.short_name),
					std::move(class_info.json)
				);
			}
			else if (class_info.parent_class_info != nullptr)
			{
				assert(class_info.parent_class_info->loaded_class != nullptr);

				class_info.loaded_class = DoCreateChildClassHelper(
					class_info.parent_class_info->loaded_class,
					manager,
					std::move(class_info.class_name),
					std::move(class_info.short_name),
					std::move(class_info.json)
				);
			}
		}
		return true;
	}

}; // namespace chaos
