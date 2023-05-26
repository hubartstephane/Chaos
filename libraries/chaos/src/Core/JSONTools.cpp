#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// =================
	// EXTERNAL FUNCTION
	// =================

	bool LoadFromJSON(nlohmann::json const& json, boost::filesystem::path& dst)
	{
		std::string result;
		if (!LoadFromJSON(json, result))
			return false;
		dst = result;
		return true;
	}

	bool SaveIntoJSON(nlohmann::json& json, boost::filesystem::path const& src)
	{
		return SaveIntoJSON(json, src.string());
	}

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

		bool ParseRecursive(char const* buffer, boost::filesystem::path const& config_path, nlohmann::json& result, LoadFileFlag flag)
		{
			assert(buffer != nullptr);
			JSONRecursiveLoader loader;
			return loader.ParseJSONFile(buffer, config_path, result, flag);
		}

		bool LoadJSONFile(FilePathParam const& path, nlohmann::json& result, LoadFileFlag flag)
		{
			if (int(flag & LoadFileFlag::RECURSIVE) != 0)
			{
				JSONRecursiveLoader loader;
				return loader.LoadJSONFile(path, result, flag);
			}
			else
			{
				Buffer<char> buffer = FileTools::LoadFile(path, LoadFileFlag::ASCII | LoadFileFlag::NO_ERROR_TRACE);
				if (buffer == nullptr)
				{
					if (int(flag & LoadFileFlag::NO_ERROR_TRACE) == 0)
					{
						Log::Error("JSONTools::LoadJSONFile: fail to load [%s]", path.GetResolvedPath().string().c_str());
					}
					return false;
				}
				return Parse(buffer.data, result);
			}
		}

		bool SaveJSONToFile(nlohmann::json const& json, FilePathParam const & path)
		{
			std::ofstream stream(path.GetResolvedPath().c_str());
			if (stream)
			{
				stream << json.dump(4);
				return true;
			}
			return false;
		}

		boost::filesystem::path DumpConfigFile(nlohmann::json const& json, char const* filename)
		{
			if (filename != nullptr)
			{
				boost::filesystem::path result;
				// get the directory to write to
				if (Application* application = Application::GetInstance())
					result = application->CreateUserLocalTempDirectory();
				else
					FileTools::CreateTemporaryDirectory("MyTempDirectory_%d", result);
				// dump to file
				if (!result.empty())
				{
					boost::filesystem::path path = result / filename;
					if (SaveJSONToFile(json, path.string().c_str()))
						return path;
				}
			}
			return {};
		}

	}; // namespace JSONTools

}; // namespace chaos
