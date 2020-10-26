#include <chaos/Chaos.h>

#if 0

#include <chaos/JSONTools.h>
#include <chaos/JSONRecursiveLoader.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>

#endif



namespace chaos
{
	// =================
	// EXTERNAL FUNCTION
	// =================

	bool LoadFromJSON(nlohmann::json const & entry, bool & dst)
	{
		try
		{
			dst = entry.get<bool>(); // for bool type
			return true;
		}
		catch (...)
		{
			try
			{
				dst = (entry.get<int>() != 0); // for int type (+ conversion to bool)
				return true;
			}
			catch (...)
			{
			}
		}
		return false;
	}

	// =================
	// JSONTools
	// =================

	namespace JSONTools
	{
		nlohmann::json* GetStructure(nlohmann::json& entry, char const* name)
		{
			if (entry.is_object())
			{
				auto it = entry.find(name);
				if (it != entry.end() && it->is_structured())
					return &*it;
			}
			return nullptr;
		}

		nlohmann::json const* GetStructure(nlohmann::json const& entry, char const* name)
		{
			if (entry.is_object())
			{
				auto it = entry.find(name);
				if (it != entry.end() && it->is_structured())
					return &*it;
			}
			return nullptr;
		}

		nlohmann::json* GetStructureByIndex(nlohmann::json& entry, size_t index)
		{
			if (entry.is_array() && index < entry.size())
			{
				nlohmann::json& result = entry[index];
				if (result.is_structured())
					return &result;
			}
			return nullptr;
		}

		nlohmann::json const* GetStructureByIndex(nlohmann::json const& entry, size_t index)
		{
			if (entry.is_array() && index < (size_t)entry.size())
			{
				nlohmann::json const& result = entry[index];
				if (result.is_structured())
					return &result;
			}
			return nullptr;
		}

		bool Parse(char const* buffer, nlohmann::json& result)
		{
			assert(buffer != nullptr);
			try
			{
				result = nlohmann::json::parse(buffer);
				return true;
			}
			catch (std::exception & error)
			{
				char const* error_string = error.what();
				return false;
			}
			catch (...)
			{
			}
			return false;
		}

		bool ParseRecursive(char const* buffer, boost::filesystem::path const& config_path, nlohmann::json& result)
		{
			assert(buffer != nullptr);
			JSONRecursiveLoader loader;
			return loader.ParseJSONFile(buffer, config_path, result);
		}

		bool LoadJSONFile(FilePathParam const& path, nlohmann::json& result, bool recursive)
		{
			if (!recursive)
			{
				Buffer<char> buffer = FileTools::LoadFile(path, true);
				if (buffer != nullptr)
					return Parse(buffer.data, result);
				return false; // loading failure
			}
			else
			{
				JSONRecursiveLoader loader;
				return loader.LoadJSONFile(path, result);
			}
		}

		boost::filesystem::path DumpConfigFile(nlohmann::json const& json, char const* filename)
		{
			boost::filesystem::path result;
			if (filename == nullptr)
				return result;

			Application* application = Application::GetInstance();
			if (application != nullptr)
			{
				result = application->CreateUserLocalTempDirectory();
			}
			else
			{
				FileTools::CreateTemporaryDirectory("MyTempDirectory_%d", result);
			}

			if (!result.empty())
			{
				boost::filesystem::path path = result / filename;
				std::ofstream stream(path.string().c_str());
				stream << json.dump(4);
			}

			return result;
		}

	}; // namespace JSONTools

}; // namespace chaos
