#pragma once

#include <chaos/StandardHeaders.h>
#include <chaos/FilePath.h>

namespace chaos
{
	class JSONTools
	{
	public:

		/** parsing a JSON file (catch exceptions) */
		static nlohmann::json Parse(char const * buffer);

		/** Load a JSON file in a recursive whay */
		static nlohmann::json LoadJSONFile(FilePathParam const & path, bool recursive = false);

		/** specialization for bool */
		static bool GetAttribute(nlohmann::json const & entry, char const * name, bool & result);
		/** specialization for bool */
		static bool GetAttribute(nlohmann::json const & entry, char const * name, bool & result, bool default_value); 

		/** reading an attribute (catch exceptions) */
		template<typename T>
		static bool GetAttribute(nlohmann::json const & entry, char const * name, T & result)
		{
			assert(name != nullptr);
			try
			{
				result = entry.value(name, result);
				return true;
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
				result = entry.value(name, default_value);
				return true;
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

    /** create a temporary directory to hold the configuration */
    static bool ShowConfigFile(nlohmann::json const & json, char const * filename = "myjson.json");
	};

}; // namespace chaos
