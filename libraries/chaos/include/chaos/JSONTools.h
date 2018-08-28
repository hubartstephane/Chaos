#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePath.h>
#include <chaos/Buffer.h>

namespace chaos
{
	class JSONTools
	{
	public:

		/** parsing a JSON file (catch exceptions) */
		static bool Parse(char const * buffer, nlohmann::json & result);
		/** parsing a JSON file from a buffer (load any dependant files) */
		static bool ParseRecursive(char const * buffer, boost::filesystem::path const & config_path, nlohmann::json & result);

		/** Load a JSON file in a recursive whay */
		static bool LoadJSONFile(FilePathParam const & path, nlohmann::json & result, bool recursive = false);

		/** specialization for bool */
		static bool GetAttribute(nlohmann::json const & entry, char const * name, bool & result);
		/** specialization for bool */
		static bool GetAttribute(nlohmann::json const & entry, char const * name, bool & result, bool default_value);

		/** specialization for bool */
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, bool & result);
		/** specialization for bool */
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, bool & result, bool default_value);

		/** reading an attribute (catch exceptions) */
		template<typename T>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result)
		{
			assert(name != nullptr);
			try
			{
				if (entry.is_object())
				{
					nlohmann::json::const_iterator it = entry.find(name);
					if (it != entry.end())
					{
						result = it->get<T>();
						return true;
					}
				}
			}
			catch (...)
			{
			}
			return false;
		}

		/** reading an attribute (catch exceptions) */
		template<typename T>
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, T & result)
		{
			try
			{
				if (entry.is_array() && index < entry.size())
				{
					result = entry[index].get<T>();
					return true;
				}
			}
			catch (...)
			{
			}
			return false;
		}

		/** reading an attribute (catch exceptions) with default value */
		template<typename T, typename Y>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result, Y default_value)
		{
			assert(name != nullptr);
			try
			{
				if (entry.is_object())
				{
					nlohmann::json::const_iterator it = entry.find(name);
					if (it != entry.end())
					{
						result = it->get<T>();
						return true;
					}
				}
			}
			catch (...)
			{

			}
			result = default_value;
			return false;
		}
		/** reading an attribute (catch exceptions) with default value */
		template<typename T, typename Y>
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, T & result, Y default_value)
		{
			assert(name != nullptr);
			try
			{
				if (entry.is_array() && index < entry.size())
				{
					result = entry[index].get<T>();
					return true;
				}
			}
			catch (...)
			{

			}
			result = default_value;
			return false;
		}

		/** get a sub object from an object */
		static nlohmann::json * GetStructure(nlohmann::json & entry, char const * name);
		/** get a sub object from an object */
		static nlohmann::json const * GetStructure(nlohmann::json const & entry, char const * name);

		/** get a sub object from an object */
		static nlohmann::json * GetStructureByIndex(nlohmann::json & entry, size_t index);
		/** get a sub object from an object */
		static nlohmann::json const * GetStructureByIndex(nlohmann::json const & entry, size_t index);

		/** create a temporary directory to hold the configuration (returns the path of the directory where the file is) */
		static boost::filesystem::path DumpConfigFile(nlohmann::json const & json, char const * filename = "myconfig.json");

		/** utility function */
		template<typename T>
		static void SaveIntoJSON(std::vector<T> const & elements, nlohmann::json & json_entries)
		{
			for (auto const & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(element, json_entry);
				json_entries.push_back(std::move(json_entry));
			}
		}
		/** utility function */
		template<typename T>
		static void SaveIntoJSON(std::vector<T*> const & elements, nlohmann::json & json_entries)
		{
			for (auto const * element : elements)
			{
				if (element == nullptr)
					continue;
				auto json_entry = nlohmann::json();
				SaveIntoJSON(*element, json_entry);
				json_entries.push_back(std::move(json_entry));
			}
		}
		/** utility function */
		template<typename T>
		static void SaveIntoJSON(std::vector<std::unique_ptr<T>> const & elements, nlohmann::json & json_entries)
		{
			for (auto & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(*element.get(), json_entry);
				json_entries.push_back(std::move(json_entry));
			}
		}

		/** utility function */
		template<typename T>
		static void LoadFromJSON(std::vector<T> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T element;
				LoadFromJSON(element, json_entry);
				elements.push_back(std::move(element));
			}
		}
		/** utility function */
		template<typename T>
		static void LoadFromJSON(std::vector<T*> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T * element(new T);
				if (element == nullptr)
					continue;
				LoadFromJSON(*element, json_entry);
				elements.push_back(std::move(element));
			}
		}
		/** utility function */
		template<typename T>
		static void LoadFromJSON(std::vector<std::unique_ptr<T>> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				std::unique_ptr<T> element(new T);
				if (element == nullptr)
					continue;
				LoadFromJSON(*element, json_entry);
				elements.push_back(std::move(element));
			}
		}

	};

}; // namespace chaos
