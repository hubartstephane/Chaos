#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePath.h>
#include <chaos/Buffer.h>
#include <chaos/SmartPointers.h>


// =================
// EXTERNAL FUNCTION
// =================


namespace chaos
{

		// ====================================================================


	/** loading a bool (because we try to read an int as a fallback) */
	bool LoadFromJSON(nlohmann::json const & entry, bool & result);
	/** default template loading (catch exceptions) */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, T & result)
	{
		result = entry.get<T>(); // may throw an exception (catched by caller)
		return true;
	}
	/** template for raw pointer */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, T * & result)
	{
		T * other = new T;
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
		{
			delete(other);
			return false;		
		}
		result = other;
		return true;
	}
	/** template for unique_ptr */
	template<typename T, typename DELETER>
	bool LoadFromJSON(nlohmann::json const & entry, std::unique_ptr<T, DELETER> & result)
	{
		std::unique_ptr<T, DELETER> other(new T); // force to use another smart pointer and swap due to lake of copy 
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
			return false;
		std::swap(result, other);
		return true;
	}
	/** template for shared_ptr */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, chaos::shared_ptr<T> & result)
	{
		result = new T;
		if (result == nullptr)
			return false;
		if (!LoadFromJSON(entry, *result))
		{
			result = nullptr;
			return false;		
		}
		return true;
	}

	/** loading specialization for vector */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & json_entries, std::vector<T> & elements)
	{
		if (!json_entries.is_array())
			return false;
		for (auto const & json_entry : json_entries)
		{
			T element;
			if (LoadFromJSON(json_entry, element))
				elements.push_back(std::move(element));
		}
		return true;
	}






	// ====================================================================


	template<typename T>
	bool SaveIntoJSON(nlohmann::json & entry, T const & result)
	{
		entry = result;
		//result = entry.get<T>(); // may throw an exception (catched by caller)
		return true;
	}




	// =================
	// JSONTools
	// =================

	class JSONTools
	{
	public:

		/** parsing a JSON file (catch exceptions) */
		static bool Parse(char const * buffer, nlohmann::json & result);
		/** parsing a JSON file from a buffer (load any dependant files) */
		static bool ParseRecursive(char const * buffer, boost::filesystem::path const & config_path, nlohmann::json & result);
		/** Load a JSON file in a recursive whay */
		static bool LoadJSONFile(FilePathParam const & path, nlohmann::json & result, bool recursive = false);
		/** create a temporary directory to hold the configuration (returns the path of the directory where the file is) */
		static boost::filesystem::path DumpConfigFile(nlohmann::json const & json, char const * filename = "myconfig.json");

		/** get a sub object from an object */
		static nlohmann::json * GetStructure(nlohmann::json & entry, char const * name);
		/** get a sub object from an object */
		static nlohmann::json const * GetStructure(nlohmann::json const & entry, char const * name);

		/** get a sub object from an object */
		static nlohmann::json * GetStructureByIndex(nlohmann::json & entry, size_t index);
		/** get a sub object from an object */
		static nlohmann::json const * GetStructureByIndex(nlohmann::json const & entry, size_t index);

	public:


		template<typename T>
		static bool SetAttribute(nlohmann::json & entry, char const * name, T const & src)
		{
			assert(name != nullptr);
			if (!entry.is_object())
				return false;
			entry[name] = nlohmann::json::array();
			try
			{


			}
			catch(...)
			{
			}
			return false;	
		}






		/** reading an attribute from a JSON structure */
		template<typename T>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result)
		{
			assert(name != nullptr);
			if (!entry.is_object())
				return false;
			nlohmann::json::const_iterator it = entry.find(name);
			if (it == entry.end())
				return false;
			try
			{
				return LoadFromJSON(*it, result);
			}
			catch (...)
			{
			}
			return false;			
		}
		/** reading an attribute from a JSON array */
		template<typename T>
		static bool JSONTools::GetAttributeByIndex(nlohmann::json const & entry, size_t index, T & result)
		{
			if (!entry.is_array() || index >= entry.size())
				return false;
			try
			{
				return LoadFromJSON(entry[index], result);
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
			if (GetAttribute(entry, name, result))
				return true;
			result = default_value;
			return false;
		}
		/** reading an attribute (catch exceptions) with default value */
		template<typename T, typename Y>
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, T & result, Y default_value)
		{
			if (GetAttributeByIndex(entry, index, result))
				return true;
			result = default_value;
			return false;
		}























		/** reading an array of elements */
		template<typename T>
		static bool GetAttributeArray(nlohmann::json const & entry, char const * name, std::vector<T> & result)
		{
			nlohmann::json const * array_json = GetStructure(entry, name);
			// read an array ?
			if (array_json != nullptr && array_json->is_array())
			{
				for (size_t i = 0; i < array_json->size(); ++i)
				{
					T tmp;
					if (JSONTools::GetAttributeByIndex(*array_json, i, tmp))
						result.push_back(std::move(tmp));
				}
				return true;
			}
			// a single element ?
			else
			{
				T tmp;
				if (GetAttribute(entry, name, tmp))
				{
					result.push_back(std::move(tmp));
					return true;
				}
			}
			return false;
		}




























		/** utility function */
		template<typename T>
		static void SaveVectorIntoJSON(std::vector<T> const & elements, nlohmann::json & json_entries)
		{
			for (auto const & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(json_entry, element);
				json_entries.push_back(std::move(json_entry));
			}
		}
		/** utility function */
		template<typename T>
		static void SaveVectorIntoJSON(std::vector<T*> const & elements, nlohmann::json & json_entries)
		{
			for (auto const * element : elements)
			{
				if (element == nullptr)
					continue;
				auto json_entry = nlohmann::json();
				SaveIntoJSON(json_entry, *element);
				json_entries.push_back(std::move(json_entry));
			}
		}
		/** utility function */
		template<typename T>
		static void SaveVectorIntoJSON(std::vector<std::unique_ptr<T>> const & elements, nlohmann::json & json_entries)
		{
			for (auto & element : elements)
			{
				auto json_entry = nlohmann::json();
				SaveIntoJSON(json_entry, *element.get());
				json_entries.push_back(std::move(json_entry));
			}
		}

	};

}; // namespace chaos
