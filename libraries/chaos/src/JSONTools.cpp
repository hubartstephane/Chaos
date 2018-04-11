#include <chaos/JSONTools.h>
#include <chaos/JSONRecursiveLoader.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>

namespace chaos
{

	bool JSONTools::GetAttribute(nlohmann::json const & entry, char const * name, bool & result)
	{
		assert(name != nullptr);
		try
		{
			if (entry.is_object())
			{
				result = (entry.value(name, 0) > 0);
				return true;
			}
		}
		catch (...)
		{				
		}
		return false;			
	}

	bool JSONTools::GetAttribute(nlohmann::json const & entry, char const * name, bool & result, bool default_value) // specialization for bool
	{			
		assert(name != nullptr);
		try
		{
			if (entry.is_object())
			{
				result = (entry.value(name, 0) > 0);
				return true;
			}
		}
		catch (...)
		{
		}
		result = default_value;
		return false;
	}

	bool JSONTools::GetAttributeByIndex(nlohmann::json const & entry, size_t index, bool & result)
	{
		try
		{
			if (entry.is_array() && index < entry.size())
			{
				result = entry[index].get<bool>();
				return true;
			}
		}
		catch (...)
		{				
		}
		return false;			
	}

	bool JSONTools::GetAttributeByIndex(nlohmann::json const & entry, size_t index, bool & result, bool default_value) // specialization for bool
	{			
		try
		{
			if (entry.is_array() && index < entry.size())
			{
				result = entry[index].get<bool>();
				return true;
			}
		}
		catch (...)
		{
		}
		result = default_value;
		return false;
	}

	nlohmann::json * JSONTools::GetStructure(nlohmann::json & entry, char const * name)
	{
		try
		{
			if (entry.is_object())
			{
				auto it = entry.find(name);
				if (it != entry.end() && it->is_structured())
					return &*it;				
			}
		}
		catch (...)
		{
		}
		return nullptr;				
	}

	nlohmann::json const * JSONTools::GetStructure(nlohmann::json const & entry, char const * name)
	{
		try
		{
			if (entry.is_object())
			{
				auto it = entry.find(name);
				if (it != entry.end() && it->is_structured())
					return &*it;					
			}
		}
		catch (...)
		{
		}
		return nullptr;				
	}

	nlohmann::json * JSONTools::GetStructureByIndex(nlohmann::json & entry, size_t index)
	{
		try
		{
			if (entry.is_array() && index < entry.size())
			{
				nlohmann::json & result = entry[index];
				if (result.is_structured())
					return &result;			
			}
		}
		catch (...)
		{
		}
		return nullptr;				
	}

	nlohmann::json const * JSONTools::GetStructureByIndex(nlohmann::json const & entry, size_t index)
	{
		try
		{
			if (entry.is_array() && index < entry.size())
			{
				nlohmann::json const & result = entry[index];
				if (result.is_structured())
					return &result;			
			}
		}
		catch (...)
		{
		}
		return nullptr;				
	}

	bool JSONTools::Parse(char const * buffer, nlohmann::json & result)
	{
		assert(buffer != nullptr);
		try
		{
			result = nlohmann::json::parse(buffer);
			return true;
		}
		catch (std::exception & error)
		{
			char const * error_string = error.what();
			return false;
		}
		catch (...)
		{
		}
		return false;
	}

	bool JSONTools::ParseRecursive(char const * buffer, boost::filesystem::path const & config_path, nlohmann::json & result)
	{
		assert(buffer != nullptr);
		JSONRecursiveLoader loader;
		return loader.ParseJSONFile(buffer, config_path, result);
	}

	bool JSONTools::LoadJSONFile(FilePathParam const & path, nlohmann::json & result, bool recursive)
	{
		if (!recursive)
		{
			Buffer<char> buffer = FileTools::LoadFile(path, true);
			if (buffer != nullptr)
				return JSONTools::Parse(buffer.data, result);
			return false; // loading failure
		}
		else
		{
			JSONRecursiveLoader loader;
			return loader.LoadJSONFile(path, result);
		}
	}

	boost::filesystem::path JSONTools::DumpConfigFile(nlohmann::json const & json, char const * filename)
	{
		boost::filesystem::path result;
		if (filename == nullptr)
			return result;

		Application * application = Application::GetInstance();
		if (application != nullptr)
		{
			result = application->CreateUserLocalTempDirectory();
		}
		else
		{
			boost::filesystem::path result_dir;
			FileTools::CreateTemporaryDirectory("MyTempDirectory_%d", result_dir);
		}

		if (!result.empty())
		{
			boost::filesystem::path path = result / filename;
			std::ofstream stream(path.string().c_str());
			stream << json.dump(4);
		}

		return result;
	}

}; // namespace chaos
