#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	// ========================================================================
	// EXTERNAL FUNCTION
	// ========================================================================

	bool PrepareSaveObjectIntoJSON(nlohmann::json* json)
	{
		if (json == nullptr)
			return false;
		if (!json->is_object())
			*json = nlohmann::json::object();
		return true;
	}

	bool PrepareSaveArrayIntoJSON(nlohmann::json* json)
	{
		if (json == nullptr)
			return false;
		if (!json->is_array())
			*json = nlohmann::json::array();
		return true;
	}

	bool SaveIntoJSON(nlohmann::json * json, boost::filesystem::path const& src)
	{
		return SaveIntoJSON(json, src.string());
	}

	namespace JSONTools
	{
		// ========================================================================
		// Node methods
		// ========================================================================

		template<typename T>
		static auto GetNodeHelper(T * json, std::string_view path)
		{
			// early exit
			if (json == nullptr)
				return (T*)nullptr;

			// node uppon which we will iterate
			T * node = json;

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
						node = nullptr;
						return true; // stop iteration on that error
					}

					auto it = node->find(subkey);
					if (it == node->end()) // found nothing
					{
						node = nullptr;
						return true; // stop iteration on that error
					}
					node = &(*it);
				}
				return false; // continue iteration
			});

			return (count > 0) ? node : (T*)nullptr; // check whether there is at least one child encoutered
		}

		nlohmann::json* GetAttributeNode(nlohmann::json * json, std::string_view path)
		{
			return GetNodeHelper(json, path);
		}

		nlohmann::json const* GetAttributeNode(nlohmann::json const * json, std::string_view path)
		{
			return GetNodeHelper(json, path);
		}

		nlohmann::json* GetOrCreateAttributeNode(nlohmann::json * json, std::string_view path)
		{
			// early exit
			if (json == nullptr)
				return nullptr;

			nlohmann::json* node = json;

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

		nlohmann::json* GetElementNode(nlohmann::json * json, size_t index)
		{
			if (json != nullptr && json->is_array() && index < json->size())
				return &json->operator [](index);
			return nullptr;
		}

		nlohmann::json const* GetElementNode(nlohmann::json const * json, size_t index)
		{
			if (json != nullptr && json->is_array() && index < json->size())
				return &json->operator [](index);
			return nullptr;
		}

		// ========================================================================
		// Object Node
		// ========================================================================

		nlohmann::json* GetAttributeObjectNode(nlohmann::json * json, std::string_view path)
		{
			if (nlohmann::json* result = GetAttributeNode(json, path))
				if (result->is_object())
					return result;
			return nullptr;
		}

		nlohmann::json const * GetAttributeObjectNode(nlohmann::json const * json, std::string_view path)
		{
			if (nlohmann::json const* result = GetAttributeNode(json, path))
				if (result->is_object())
					return result;
			return nullptr;
		}

		nlohmann::json* GetOrCreateAttributeObjectNode(nlohmann::json * json, std::string_view path)
		{
			if (nlohmann::json* result = GetOrCreateAttributeNode(json, path))
			{
				if (result->is_null())
					*result = nlohmann::json::object();
				if (result->is_object())
					return result;
			}
			return nullptr;
		}

		nlohmann::json * GetElementObjectNode(nlohmann::json * json, size_t index)
		{
			if (nlohmann::json * result = GetElementNode(json, index))
				if (result->is_object())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetElementObjectNode(nlohmann::json const * json, size_t index)
		{
			if (nlohmann::json const* result = GetElementNode(json, index))
				if (result->is_object())
					return result;
			return nullptr;
		}

		// ========================================================================
		// Array node
		// ========================================================================

		nlohmann::json* GetElementArrayNode(nlohmann::json * json, std::string_view path)
		{
			if (nlohmann::json* result = GetAttributeNode(json, path))
				if (result->is_array())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetElementArrayNode(nlohmann::json const * json, std::string_view path)
		{
			if (nlohmann::json const* result = GetAttributeNode(json, path))
				if (result->is_array())
					return result;
			return nullptr;
		}

		nlohmann::json* GetOrCreateAttributeArrayNode(nlohmann::json * json, std::string_view path)
		{
			if (nlohmann::json* result = GetOrCreateAttributeNode(json, path))
			{
				if (result->is_null())
					*result = nlohmann::json::array();
				if (result->is_array())
					return result;
			}
			return nullptr;
		}

		nlohmann::json* GetElementArrayNode(nlohmann::json * json, size_t index)
		{
			if (nlohmann::json* result = GetElementNode(json, index))
				if (result->is_array())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetElementArrayNode(nlohmann::json const * json, size_t index)
		{
			if (nlohmann::json const* result = GetElementNode(json, index))
				if (result->is_array())
					return result;
			return nullptr;
		}

		// ========================================================================
		// Structure node
		// ========================================================================

		nlohmann::json* GetAttributeStructureNode(nlohmann::json * json, std::string_view path)
		{
			if (nlohmann::json* result = GetAttributeNode(json, path))
				if (result->is_structured())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetAttributeStructureNode(nlohmann::json const * json, std::string_view path)
		{
			if (nlohmann::json const* result = GetAttributeNode(json, path))
				if (result->is_structured())
					return result;
			return nullptr;
		}

		nlohmann::json* GetElementStructureNode(nlohmann::json * json, size_t index)
		{
			if (nlohmann::json* result = GetElementNode(json, index))
				if (result->is_structured())
					return result;
			return nullptr;
		}

		nlohmann::json const* GetElementStructureNode(nlohmann::json const * json, size_t index)
		{
			if (nlohmann::json const* result = GetElementNode(json, index))
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
			if (HasAnyFlags(flag, LoadFileFlag::RECURSIVE))
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
						JsonLog::Error("JSONTools::LoadJSONFile: fail to load [%s]", path.GetResolvedPath().string().c_str());
					}
					return false;
				}
				return Parse(buffer.data, result);
			}
		}

		bool SaveJSONToFile(nlohmann::json const * json, FilePathParam const& path)
		{
			if (json == nullptr)
				return false;

			std::ofstream stream(path.GetResolvedPath().c_str());
			if (stream)
			{
				stream << json->dump(4);
				return true;
			}
			return false;
		}

		boost::filesystem::path DumpConfigFile(nlohmann::json const * json, char const* filename)
		{
			if (json != nullptr && filename != nullptr)
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
