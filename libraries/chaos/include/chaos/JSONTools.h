#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePath.h>
#include <chaos/Buffer.h>


// =================
// EXTERNAL FUNCTION
// =================


namespace chaos
{


	/** specialization for bool (because we try to read an int as a fallback) */
	bool LoadFromJSON(nlohmann::json const & entry, bool & result);
	/** reading an attribute (catch exceptions) */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, T & result)
	{
		result = entry.get<T>(); // may throw an exception (catched by caller)
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





	protected:




	public:



































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

















		/** reading a GLM vector */
		template<typename VECTOR_TYPE>
		static bool GetVector(nlohmann::json const & entry, VECTOR_TYPE & result)
		{
			if (!entry.is_array())
				return false;
			size_t count = entry.size();
			for (size_t i = 0; (i < count) && (i < (size_t)result.length()) ; ++i)
				result[i] = entry[i].get<VECTOR_TYPE::value_type>();
			return true;
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

		/** utility function */
		template<typename T>
		static void LoadVectorFromJSON(std::vector<T> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T element;
				LoadFromJSON(json_entry, element);
				elements.push_back(std::move(element));
			}
		}
		/** utility function */
		template<typename T>
		static void LoadVectorFromJSON(std::vector<T*> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				T * element(new T);
				if (element == nullptr)
					continue;
				LoadFromJSON(json_entry, *element);
				elements.push_back(std::move(element));
			}
		}
		/** utility function */
		template<typename T>
		static void LoadVectorFromJSON(std::vector<std::unique_ptr<T>> & elements, nlohmann::json const & json_entries)
		{
			for (auto const & json_entry : json_entries)
			{
				std::unique_ptr<T> element(new T);
				if (element == nullptr)
					continue;
				LoadFromJSON(json_entry, *element);
				elements.push_back(std::move(element));
			}
		}








		//template<typename T>
		//static void LoadObjectFromJSON(nlohmann::json & entry, char const * name)




	};

}; // namespace chaos
