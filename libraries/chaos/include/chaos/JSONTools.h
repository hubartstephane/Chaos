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
	/** loading a bool (because we try to read an int as a fallback) */
	bool LoadFromJSON(nlohmann::json const & entry, bool & dst);
	/** default template loading (catch exceptions) */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, T & dst)
	{
		try
		{
			dst = entry.get<T>(); // may throw an exception
			return true;
		}
		catch (...)
		{
		}
		return false;
	}
	/** template for raw pointer */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, T * & dst)
	{
		T * other = new T;
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
		{
			delete(other);
			return false;		
		}
		dst = other;
		return true;
	}
	/** template for unique_ptr */
	template<typename T, typename DELETER>
	bool LoadFromJSON(nlohmann::json const & entry, std::unique_ptr<T, DELETER> & dst)
	{
		std::unique_ptr<T, DELETER> other(new T); // force to use another smart pointer and swap due to lake of copy 
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
			return false;
		std::swap(dst, other);
		return true;
	}
	/** template for shared_ptr */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, chaos::shared_ptr<T> & dst)
	{
		chaos::shared_ptr<T> other = new T;
		if (other == nullptr)
			return false;
		if (!LoadFromJSON(entry, *other))
			return false;
		dst = other;
		return true;
	}

	/** loading specialization for vector */
	template<typename T>
	bool LoadFromJSON(nlohmann::json const & entry, std::vector<T> & dst)
	{
		// input is an array
		if (entry.is_array())
		{
			for (auto const & json_entry : entry)
			{
				T element;
				if (LoadFromJSON(json_entry, element))
					dst.push_back(std::move(element));
			}
			return true;
		}
		// considere input as an array of a single element
		T element;
		if (!LoadFromJSON(entry, element))
			return false;
		dst.push_back(std::move(element));
		return true;
	}

	template<typename T>
	bool SaveIntoJSON(nlohmann::json & entry, T src) // copy for basic types
	{
		try
		{
			entry = src;
			return true;
		}
		catch (...)
		{			
		}	
		return false;
	}
	/** template for raw pointer */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json & entry, T const * src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(entry, *src);
	}
	/** template for unique_ptr */
	template<typename T, typename DELETER>
	bool SaveIntoJSON(nlohmann::json & entry, std::unique_ptr<T, DELETER> const & src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(entry, *src);
	}
	/** template for shared_ptr */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json & entry, chaos::shared_ptr<T> const & src)
	{
		if (src == nullptr)
			return true;
		return SaveIntoJSON(entry, *src);
	}

	/** specialization for vector */
	template<typename T>
	bool SaveIntoJSON(nlohmann::json & entry, std::vector<T> const & src)
	{
		entry = nlohmann::json::array();
		for (auto const & element : src)
		{
			nlohmann::json j;
			if (SaveIntoJSON(j, element))
				entry.push_back(std::move(j));
		}
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

		/** set an attribute in a json structure */
		template<typename T>
		static bool SetAttribute(nlohmann::json & entry, char const * name, T const & src)
		{
			assert(name != nullptr);
			if (entry.is_null())
				entry = nlohmann::json::object();
			else if (!entry.is_object())
				return false;
			entry[name] = nlohmann::json();
			return SaveIntoJSON(entry[name], src);
		}

		/** set an attribute in a json array */
		template<typename T>
		static bool SetAttributeByIndex(nlohmann::json & entry, size_t index, T const & src)
		{
			if (entry.is_null())
				entry = nlohmann::json::array();
			else if (!entry.is_array())
				return false;
			entry[index] = nlohmann::json();
			return SaveIntoJSON(entry[index], src);
		}

		/** set an attribute in a json structure with a lookup table */
		template<typename T, typename ENCODE_TABLE>
		static bool SetEnumAttribute(nlohmann::json & entry, char const * name, ENCODE_TABLE const & encode_table, T const & src)
		{
			std::string encoded_src;
			if (!EncodeEnum(src, encode_table, encoded_src))
				return false;
			return SetAttribute(entry, name, encoded_src);
		}

		/** set an attribute in a json structure with a lookup table */
		template<typename T, typename ENCODE_TABLE>
		static bool SetEnumAttributeByIndex(nlohmann::json & entry, size_t index, ENCODE_TABLE const & encode_table, T const & src)
		{
			std::string encoded_src;
			if (!EncodeEnum(src, encode_table, encoded_src))
				return false;
			return SetAttributeByIndex(entry, index, encoded_src);
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
			return LoadFromJSON(*it, result);
		}

		/** reading an attribute from a JSON array */
		template<typename T>
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, T & result)
		{
			if (!entry.is_array() || index >= entry.size())
				return false;
			return LoadFromJSON(entry[index], result);
		}

		/** reading an attribute with default value */
		template<typename T, typename Y>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result, Y default_value)
		{
			if (GetAttribute(entry, name, result))
				return true;
			result = default_value;
			return false;
		}

		/** reading an attribute with default value */
		template<typename T, typename Y>
		static bool GetAttributeByIndex(nlohmann::json const & entry, size_t index, T & result, Y default_value)
		{
			if (GetAttributeByIndex(entry, index, result))
				return true;
			result = default_value;
			return false;
		}

		/** reading an attribute and make a lookup on an encoding table */
		template<typename T, typename ENCODE_TABLE>
		static bool GetEnumAttribute(nlohmann::json const & entry, char const * name, ENCODE_TABLE const & encode_table, T & result)
		{
			std::string str_result;
			if (!GetAttribute(entry, name, str_result))
				return false;
			return DecodeEnum(str_result.c_str(), encode_table, result);
		}

		/** reading an attribute and make a lookup on an encoding table */
		template<typename T, typename ENCODE_TABLE>
		static bool GetEnumAttributeByIndex(nlohmann::json const & entry, size_t index, ENCODE_TABLE const & encode_table, T & result)
		{
			std::string str_result;
			if (!GetAttributeByIndex(entry, index, str_result))
				return false;
			return DecodeEnum(str_result.c_str(), encode_table, result);
		}

		/** reading an attribute and make a lookup on an encoding table with a default value */
		template<typename T, typename ENCODE_TABLE, typename Y>
		static bool GetEnumAttribute(nlohmann::json const & entry, char const * name, ENCODE_TABLE const & encode_table, T & result, Y default_value)
		{
			if (GetEnumAttribute(entry, name, encode_table, result))
				return true;
			result = default_value;
			return false;
		}

		/** reading an attribute and make a lookup on an encoding table with a default value */
		template<typename T, typename ENCODE_TABLE, typename Y>
		static bool GetEnumAttributeByIndex(nlohmann::json const & entry, size_t index, ENCODE_TABLE const & encode_table, T & result, Y default_value)
		{
			if (GetEnumAttributeByIndex(entry, index, encode_table, result))
				return true;
			result = default_value;
			return false;
		}

	protected:

		/** decode a value with a conversion table */
		template<typename T, typename ENCODE_TABLE>
		static bool DecodeEnum(char const * str, ENCODE_TABLE const & encode_table, T & result)
		{
			for (auto const & encode : encode_table)
			{
				if (StringTools::Stricmp(str, encode.second) == 0)
				{
					result = encode.first;
					return true;
				}
			}
			return false;
		}

		/** encode a value with a conversion table */
		template<typename T, typename ENCODE_TABLE>
		static bool EncodeEnum(T const & src, ENCODE_TABLE const & encode_table, std::string & result)
		{
			for (auto const & encode : encode_table)
			{
				if (src == encode.first)
				{
					result = encode.second;
					return true;
				}
			}
			return false;
		}



	};

}; // namespace chaos
