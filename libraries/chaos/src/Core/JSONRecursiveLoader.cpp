#include <chaos/Chaos.h>

namespace chaos
{


	bool JSONRecursiveLoader::LoadJSONFile(FilePathParam const & path, nlohmann::json & result)
	{
		ComputeSubstitutionChain(path);
		return FinalizeSubstitutions(result);
	}

	bool JSONRecursiveLoader::ParseJSONFile(char const * buffer, boost::filesystem::path const & config_path, nlohmann::json & result)
	{
		ComputeSubstitutionChain(buffer, config_path);
		return FinalizeSubstitutions(result);
	}

	bool JSONRecursiveLoader::FinalizeSubstitutions(nlohmann::json & result)
	{
		if (entries.size() > 0)
		{
			MakeSubstitutions();
			result = std::move(entries[0]->json);
			Clear();
			return true;
		}
		return false;
	}

	void JSONRecursiveLoader::ComputeSubstitutionChain(char const * buffer, boost::filesystem::path const & config_path)
	{
		ComputeSubstitutionChainHelper(CreateEntry(buffer, config_path));
	}

	void JSONRecursiveLoader::ComputeSubstitutionChain(FilePathParam const & path)
	{
		ComputeSubstitutionChainHelper(CreateEntry(path));
	}

	void JSONRecursiveLoader::ComputeSubstitutionChainHelper(LoaderEntry * entry)
	{
		if (entry == nullptr)
			return;
		stacked_entries.push_back(entry);
		DoComputeSubstitutionChain(entry, entry->json);
		stacked_entries.pop_back();
	}

	bool JSONRecursiveLoader::DoMakeStringSubstitution(LoaderEntry * entry, std::string & result)
	{
		if (result[0] == '@') // may be a path ?
		{
			if (result[1] == '@') // escaped characters ?
			{
				result = result.c_str() + 1; // skip the first
			}
			else
			{
				FilePathParam replacement_path(result.c_str() + 1, entry->path);
				result = replacement_path.GetResolvedPath().string().c_str();
			}
			return true;
		}
		return false;
	}

	void JSONRecursiveLoader::MakeStringSubstitution(LoaderEntry * entry, nlohmann::json & root)
	{
		std::string str = root.get<std::string>();
		if (DoMakeStringSubstitution(entry, str))
			root = str;
	}

	void JSONRecursiveLoader::DoComputeSubstitutionChain(LoaderEntry * entry, nlohmann::json & root)
	{
		assert(entry != nullptr);
		if (root.is_string())
		{
			MakeStringSubstitution(entry, root);
		}
		else if (root.is_object() || root.is_array())
		{
			if (root.is_object())
			{
				nlohmann::json::iterator filename_it = root.find("@include"); // replace any object that contains "@include" = "..."
				if (filename_it != root.end())
				{
					if (filename_it->is_string())
					{
						std::string str = filename_it->get<std::string>();
						DoMakeStringSubstitution(entry, str);
						FilePathParam replacement_path(str, entry->path);

						boost::filesystem::path const & resolved_path = replacement_path.GetResolvedPath();

						bool infinite_recursion = false;
						LoaderEntry * new_entry = FindOrCreateEntry(replacement_path, infinite_recursion);
						if (infinite_recursion)
						{
#if _DEBUG
							root["INFINITE_RECURSION"] = true;
#endif
						}
						else if (new_entry != nullptr)
						{
							// insert a substitution
							new_entry->to_replaced_nodes.push_back(&root);
							// recurse to new file
							stacked_entries.push_back(new_entry);
							DoComputeSubstitutionChain(new_entry, new_entry->json);
							stacked_entries.pop_back();
						}
						else
						{
#if _DEBUG
							root["LOADING_ERROR"] = true;
#endif
						}
						return;
					}
				}
			}
			// recursive part for array and objects
			for (nlohmann::json::iterator it = root.begin(); it != root.end(); ++it)
				DoComputeSubstitutionChain(entry, *it);
		}
	}

	void JSONRecursiveLoader::Clear()
	{
		size_t count = entries.size();
		for (size_t i = 0; i < count; ++i)
			delete(entries[i]);
		entries.clear();
		stacked_entries.clear();
	}

	JSONRecursiveLoader::LoaderEntry * JSONRecursiveLoader::FindEntry(FilePathParam const & path)
	{
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();
		for (size_t i = 0; i < entries.size(); ++i)
			if (entries[i]->path == resolved_path)
				return entries[i];
		return nullptr;
	}

	JSONRecursiveLoader::LoaderEntry * JSONRecursiveLoader::FindOrCreateEntry(FilePathParam const & path, bool & infinite_recursion)
	{
		infinite_recursion = false;

		LoaderEntry * result = FindEntry(path);
		if (result != nullptr)
		{
			if (std::find(stacked_entries.begin(), stacked_entries.end(), result) != stacked_entries.end())
			{
				infinite_recursion = true;
				return nullptr; // infinite recursion detected !
			}
			return result;
		}
		return CreateEntry(path);
	}

	JSONRecursiveLoader::LoaderEntry * JSONRecursiveLoader::CreateEntry(FilePathParam const & path)
	{
		nlohmann::json new_json;
		if (!JSONTools::LoadJSONFile(path, new_json, false))
			return nullptr;
		return DoCreateEntry(new_json, path.GetResolvedPath());
	}

	JSONRecursiveLoader::LoaderEntry * JSONRecursiveLoader::CreateEntry(char const * buffer, boost::filesystem::path const & config_path)
	{
		nlohmann::json new_json;
		if (!JSONTools::Parse(buffer, new_json))
			return nullptr;
		return DoCreateEntry(new_json, config_path);
	}

	JSONRecursiveLoader::LoaderEntry * JSONRecursiveLoader::DoCreateEntry(nlohmann::json & new_json, boost::filesystem::path const & config_path)
	{
		LoaderEntry * new_entry = new LoaderEntry();
		if (new_entry == nullptr)
			return nullptr;
		new_entry->json = std::move(new_json);
		new_entry->path = config_path;
		entries.push_back(new_entry);
		return new_entry;
	}

	void JSONRecursiveLoader::MakeSubstitutions()
	{
		for (size_t i = entries.size(); i > 0; --i) // substitutions are done in reverse order to ensure dependancy management
		{
			LoaderEntry * entry = entries[i - 1];

			for (size_t j = 0; j < entry->to_replaced_nodes.size(); ++j)
			{
				if (j == entry->to_replaced_nodes.size() - 1) // make a MOVE operation for the last substitution
					*entry->to_replaced_nodes[j] = std::move(entry->json);
				else // make a COPY operation for every other substitution
					*entry->to_replaced_nodes[j] = entry->json;
			}
		}
	}

}; // namespace chaos
