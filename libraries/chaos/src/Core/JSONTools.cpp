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

	// =================
	// JSONTools
	// =================

	namespace JSONTools
	{
		namespace details
		{
			template<typename T>
			static auto GetStructureInternalHelper(T & entry, char const* name)
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

				StringTools::WithSplittedText(name, "/", [&node, &count](char const* subkey)
				{
					if (!StringTools::IsEmpty(subkey))
					{
						// early exit if type does not correspond
						if (!node->is_object())
						{
							node = result_type(nullptr);
							return true; // stop iteration on that error
						}

						auto it = node->find(subkey);
						if (it == node->end() || !it->is_structured()) // found nothing or something that is not a structure
						{
							node = result_type(nullptr);
							return true; // stop iteration on that error
						}
						node = &(*it);
						++count;
					}
					return false; // continue iteration
				});

				return (count > 0) ? node : result_type(nullptr); // check whether there is at least one child encoutered
			}

			nlohmann::json* GetStructureInternal(nlohmann::json& entry, char const* name)
			{
				return GetStructureInternalHelper(entry, name);
			}

			nlohmann::json const* GetStructureInternal(nlohmann::json const& entry, char const* name)
			{
				return GetStructureInternalHelper(entry, name);
			}

			nlohmann::json* GetOrCreateStructureInternal(nlohmann::json& json, char const* name)
			{
				nlohmann::json* node = &json;

				int count = 0;
				bool creation_mode = false; // once a node has been created, it is unnecessary to make search again

				StringTools::WithSplittedText(name, "/", [&node, &count, &creation_mode](char const* subkey)
				{
					if (!StringTools::IsEmpty(subkey))
					{
						++count;

						// search whether the node already exists
						if (!creation_mode)
						{
							// early exit if type does not correspond
							if (!node->is_object())
							{
								node = nullptr;
								return true; // stop iteration on that error
							}

							auto it = node->find(subkey);
							if (it == node->end())
							{
								creation_mode = true; // node does not exists. all subsequent children are to be created
							}
							else if (!it->is_structured()) // node found but it is not a structure
							{
								node = nullptr;
								return true; // stop iteration on that error
							}
						}

						// create new node
						node = &node->operator [](subkey);
						*node = nlohmann::json::object();

						//node->operator [](subkey) = nlohmann::json::object();
						//child_structure = &json[p.data()];

					}
					return false; // continue iteration
				});

				return (count > 0) ? node : nullptr;
			}

		}; // namespace details

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
