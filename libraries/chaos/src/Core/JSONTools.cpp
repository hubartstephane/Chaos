#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// EXTERNAL FUNCTION
	// ========================================================================

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

	bool LoadFromJSON(nlohmann::json const& entry, bool& dst)
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

	namespace JSONTools
	{
		// ========================================================================
		// Node methods
		// ========================================================================

		template<typename T>
		static auto GetNodeHelper(T& entry, std::string_view path)
		{
			using result_type = typename std::remove_reference<T>::type *;

			// node uppon which we will iterate
			result_type node = &entry;

			// we have to have at least one non-empty subkey for a valid indirection
			// for ex:
			//   -"A"      is a valid key
			//   -""       is NOT a valid key
			//   -"A/B//C" is a valid key
			//   -"///"    is NOT a valid key
			int count = 0; 

			StringTools::WithSplittedText(path, "/", [&node, &count](char const* subkey)
			{
				if (!StringTools::IsEmpty(subkey))
				{
					++count;

					// early exit if type does not correspond
					if (!node->is_object())
					{
						node = result_type(nullptr);
						return true; // stop iteration on that error
					}

					auto it = node->find(subkey);
					if (it == node->end()) // found nothing
					{
						node = result_type(nullptr);
						return true; // stop iteration on that error
					}
					node = &(*it);
				}
				return false; // continue iteration
			});

			return (count > 0) ? node : result_type(nullptr); // check whether there is at least one child encoutered
		}

		nlohmann::json* GetNode(nlohmann::json& entry, std::string_view path)
		{
			return GetNodeHelper(entry, path);
		}

		nlohmann::json const* GetNode(nlohmann::json const& entry, std::string_view path)
		{
			return GetNodeHelper(entry, path);
		}

		nlohmann::json* GetOrCreateNode(nlohmann::json& entry, std::string_view path)
		{
			nlohmann::json* node = &entry;

			int count = 0;

			StringTools::WithSplittedText(path, "/", [&node, &count](char const* subkey)
			{
				if (!StringTools::IsEmpty(subkey))
				{
					++count;

					// object -> search child
					if (node->is_object())
					{
						// search child by name
						auto it = node->find(subkey);
						if (it != node->end())
						{
							node = &(*it);
							return false; // continue iteration
						}
					}
					// empty node -> create an object
					else if(node->is_null())
					{
						*node = nlohmann::json::object();
					}
					// error if type does not correspond
					else
					{
						node = nullptr;
						return true; // stop iteration on that error
					}
					// create a new node
					node = &node->operator [](subkey);
				}
				return false; // continue iteration
			});
			return (count > 0) ? node : nullptr;
		}

		nlohmann::json* GetNodeByIndex(nlohmann::json& entry, size_t index)
		{
			if (entry.is_array() && index < entry.size())
				return &entry[index];
			return nullptr;
		}

		nlohmann::json const* GetNodeByIndex(nlohmann::json const& entry, size_t index)
		{
			if (entry.is_array() && index < entry.size())
				return &entry[index];
			return nullptr;
		}

		// ========================================================================
		// Object Node
		// ========================================================================

		nlohmann::json* GetObjectNode(nlohmann::json& json, std::string_view path)
		{
			if (nlohmann::json* result = GetNode(json, path))
				if (result->is_object())
					return result;
			return nullptr;
		}

		nlohmann::json const * GetObjectNode(nlohmann::json const& json, std::string_view path)
		{
			if (nlohmann::json const* result = GetNode(json, path))
				if (result->is_object())
					return result;
			return nullptr;
		}

		nlohmann::json* GetOrCreateObjectNode(nlohmann::json& json, std::string_view path)
		{
			if (nlohmann::json* result = GetOrCreateNode(json, path))
			{				
				if (result->is_null())
					*result = nlohmann::json::object();
				if (result->is_object())
					return result;			
			}
			return nullptr;
		}

		nlohmann::json * GetObjectNodeByIndex(nlohmann::json & entry, size_t index)
		{
			if (nlohmann::json * result = GetNodeByIndex(entry, index))
				if (result->is_object())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetObjectNodeByIndex(nlohmann::json const& entry, size_t index)
		{
			if (nlohmann::json const* result = GetNodeByIndex(entry, index))
				if (result->is_object())
					return result;
			return nullptr;
		}

		// ========================================================================
		// Array node
		// ========================================================================

		nlohmann::json* GetArrayNode(nlohmann::json& json, std::string_view path)
		{
			if (nlohmann::json* result = GetNode(json, path))
				if (result->is_array())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetArrayNode(nlohmann::json const& json, std::string_view path)
		{
			if (nlohmann::json const* result = GetNode(json, path))
				if (result->is_array())
					return result;
			return nullptr;
		}

		nlohmann::json* GetOrCreateArrayNode(nlohmann::json& json, std::string_view path)
		{
			if (nlohmann::json* result = GetOrCreateNode(json, path))
			{
				if (result->is_null())
					*result = nlohmann::json::array();
				if (result->is_array())
					return result;
			}
			return nullptr;
		}

		nlohmann::json* GetArrayNodeByIndex(nlohmann::json& entry, size_t index)
		{
			if (nlohmann::json* result = GetNodeByIndex(entry, index))
				if (result->is_array())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetArrayNodeByIndex(nlohmann::json const& entry, size_t index)
		{
			if (nlohmann::json const* result = GetNodeByIndex(entry, index))
				if (result->is_array())
					return result;
			return nullptr;
		}

		// ========================================================================
		// Structure node
		// ========================================================================

		nlohmann::json* GetStructureNode(nlohmann::json& json, std::string_view path)
		{
			if (nlohmann::json* result = GetNode(json, path))
				if (result->is_structured())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetStructureNode(nlohmann::json const& json, std::string_view path)
		{
			if (nlohmann::json const* result = GetNode(json, path))
				if (result->is_structured())
					return result;
			return nullptr;
		}

		nlohmann::json* GetStructureNodeByIndex(nlohmann::json& entry, size_t index)
		{
			if (nlohmann::json* result = GetNodeByIndex(entry, index))
				if (result->is_structured())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetStructureNodeByIndex(nlohmann::json const& entry, size_t index)
		{
			if (nlohmann::json const* result = GetNodeByIndex(entry, index))
				if (result->is_structured())
					return result;
			return nullptr;
		}
		// ========================================================================
		// Miscellaneous
		// ========================================================================

		bool Parse(char const* buffer, nlohmann::json& result)
		{
			assert(buffer != nullptr);
			try
			{
				result = nlohmann::json::parse(buffer);
				return true;
			}
			catch (std::exception& error)
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
			if ((flag & LoadFileFlag::RECURSIVE) != LoadFileFlag::NONE)
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

		bool SaveJSONToFile(nlohmann::json const& json, FilePathParam const& path)
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
