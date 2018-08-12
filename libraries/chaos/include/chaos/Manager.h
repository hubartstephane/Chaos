#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/ReferencedObject.h>
#include <chaos/FilePath.h>
#include <chaos/JSONTools.h>

namespace chaos
{

	// ==============================================================
	// MANAGER
	// ==============================================================

	class Manager : public ReferencedObject
	{

	public:

		/** start the manager */
		bool StartManager();
		/** stop the manager */
		bool StopManager();

		/** whether the manager is started */
		bool IsManagerStarted() const;

		/** initialize the manager from a configuration file */
		virtual bool InitializeFromConfiguration(nlohmann::json const & config, boost::filesystem::path const & config_path);

	protected:

		/** internally starts the manager */
		virtual bool DoStartManager();
		/** internally stops the manager */
		virtual bool DoStopManager();


		/** an utility method to initialize a single object in an JSON array/object */
		template<typename FUNC>
		auto InitializeOneObjectFromConfiguration(char const * keyname, nlohmann::json const & json, boost::filesystem::path const & config_path, FUNC & add_func)
		{
			// 1 - we receive a key and its is valid (starts with '@')
			if (keyname != nullptr && keyname[0] == '@' && keyname[1] != 0)
				return add_func(keyname + 1, json, config_path);
			// 2 - try to find a member 'name'
			nlohmann::json::const_iterator name_json_it = json.find("name");			
			if (name_json_it != json.end() && name_json_it->is_string())
			{
				std::string name = name_json_it->get<std::string>();
				if (!name.empty())
					return add_func(name.c_str(), json, config_path);
			}
			// 3 - anonymous object
			return add_func(nullptr, json, config_path);
		}

		/** an utility method to initialize a list of objects from a JSON object or array */
		template<typename FUNC>
		bool InitializeObjectsFromConfiguration(char const * object_names, nlohmann::json const & json, boost::filesystem::path const & config_path, FUNC & add_func)
		{
			nlohmann::json const * objects_json = JSONTools::GetStructure(json, object_names);
			if (objects_json != nullptr)
			{
				for (nlohmann::json::const_iterator it = objects_json->begin(); it != objects_json->end(); ++it)
				{
					if (objects_json->is_array())
						InitializeOneObjectFromConfiguration(nullptr, *it, config_path, add_func);
					else if (objects_json->is_object())
						InitializeOneObjectFromConfiguration(it.key().c_str(), *it, config_path, add_func);
				}
			}
			return true;
		}

		/** utility function to remove an object from a list */
		template<typename T, typename FUNC>
		void DoRemoveObject(size_t index, T & vector, FUNC remove_func)
		{
			// ensure the index is valid
			size_t count = vector.size();
			if (index >= count)
				return;
			// copy the intrusive_ptr to prevent the destruction
			auto object = vector[index];
			// remove the object from the array
			if (index != count - 1)
				vector[index] = vector[count - 1];
			vector.pop_back();
			// callback then let the unreferencement manage the object lifetime
			remove_func(object.get());
		}

		/** detach all elements from a list */
		template<typename T, typename FUNC>
		void RemoveAllObjectsFromList(T & vector, FUNC remove_func)
		{
			while (vector.size() > 0)
			{
				// copy the intrusive_ptr to prevent the destruction
				auto object = vector.back();
				// remove the object from the array
				vector.pop_back();
				// callback then let the unreferencement manage the object lifetime
				remove_func(object.get());
			}
		}

		template<typename T, typename U>
		static size_t FindObjectIndexInVector(T * object, U const & vector)
		{
			assert(object != nullptr);
			size_t count = vector.size();
			for (size_t i = 0; i < count; ++i)
				if (vector[i].get() == object)
					return i;
			return count;
		}
		/** a generic function to find an object in a list by its name */
		template<typename T, typename U>
		static T * FindObjectByName(char const * name, U & objects)
		{
			if (name == nullptr)
				return nullptr;

			size_t count = objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				T * object = objects[i].get();
				if (object == nullptr)
					continue;
				if (strcmp(object->GetName(), name) == 0)
					return object;
			}
			return nullptr;
		}

		/** a generic function to find an object in a list by its path */
		template<typename T, typename U>
		static T * FindObjectByPath(FilePathParam const & in_path, U & objects)
		{
			boost::filesystem::path const & resolved_path = in_path.GetResolvedPath();

			size_t count = objects.size();
			for (size_t i = 0; i < count; ++i)
			{
				T * obj = objects[i].get();
				if (obj == nullptr)
					continue;
				if (obj->GetPath() == resolved_path)
					return obj;
			}
			return nullptr;
		}

		/** utility function to test whether an object can be inserted */
		template<typename FUNC>
		bool CanAddObject(char const * name, FUNC find_func) const
		{
			// manager initialized ?
			if (!IsManagerStarted())
				return false;
			// name already existing ?
			if (name != nullptr && find_func(name) != nullptr)
				return false;
			return true;
		}

	protected:

		/** whether the manager is started */
		bool manager_started = false;
	};

}; // namespace chaos
