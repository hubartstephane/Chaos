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
		static nlohmann::json LoadJSONFile(FilePathParam const & path, bool recursive = true);

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
