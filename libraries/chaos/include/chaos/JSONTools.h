#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePath.h>

namespace chaos
{
	class JSONTools
	{
	public:


#if 0

    static bool Parse(char const * buffer, rapidjson::Document & doc);

    static bool LoadJSONFileRecursive(FilePathParam const & path, rapidjson::Document & doc);

    static bool GetAttribute(rapidjson::Document::Object const & obj, char const * name, bool & result, bool default_value = false);

    static bool GetAttribute(rapidjson::Document::Object const & obj, char const * name, float & result, float default_value = 0.0f);

    static bool GetAttribute(rapidjson::Document::Object const & obj, char const * name, double & result, double default_value = 0.0);

    static bool GetAttribute(rapidjson::Document::Object const & obj, char const * name, int & result, int default_value = 0);

    static bool GetAttribute(rapidjson::Document::Object const & obj, char const * name, std::string & result);

#endif






		/** parsing a JSON file (catch exceptions) */
		static nlohmann::json Parse(char const * buffer);

    /** Load a JSON file in a recursive whay */
    static nlohmann::json LoadJSONFileRecursive(FilePathParam const & path);

		/** reading an attribute (catch exceptions) */
		template<typename T>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result)
		{
			assert(name != nullptr);
			try
			{
				result = entry.value(name, result);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}
		static bool GetAttribute(nlohmann::json const & entry, char const * name, bool & result) // specialization for bool
		{
			assert(name != nullptr);
			try
			{
				result = (entry.value(name, 0) > 0);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}
		/** reading an attribute (catch exceptions) with default value */
		template<typename T, typename Y>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result, Y default_value)
		{
			assert(name != nullptr);
			try
			{
				result = entry.value(name, default_value);
			}
			catch (...)
			{
				result = default_value;
				return false;
			}
			return true;
		}
		static bool GetAttribute(nlohmann::json const & entry, char const * name, bool & result, bool default_value) // specialization for bool
		{
			result = default_value;
			assert(name != nullptr);
			try
			{
				result = (entry.value(name, 0) > 0);
			}
			catch (...)
			{
				return false;
			}
			return true;
		}

		static nlohmann::json GetStructure(nlohmann::json const & entry, char const * name)
		{
			nlohmann::json result;
			try
			{
				if (entry.is_structured())
				{
					auto it = entry.find(name);
					if (it != entry.end())
						return *it;				
				}
			}
			catch (...)
			{

			}
			return result;				
		}
	};

}; // namespace chaos
