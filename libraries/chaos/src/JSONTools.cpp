#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>
#include <chaos/WinTools.h>

namespace chaos
{

	nlohmann::json JSONTools::Parse(char const * buffer)
	{
		assert(buffer != nullptr);
		try
		{
			return nlohmann::json::parse(buffer);
		} 
		catch (...)
		{
		}
		return nlohmann::json();
	}

	bool JSONTools::GetAttribute(nlohmann::json const & entry, char const * name, bool & result)
	{
		assert(name != nullptr);
		try
		{
			result = (entry.value(name, 0) > 0);
			return true;
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
			result = (entry.value(name, 0) > 0);
			return true;
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
				if (it != entry.end() && it->is_object())
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

	class JSONRecursiveLoader
	{
		class LoaderEntry
		{
		public:
			/** the path corresponding to this object */
			boost::filesystem::path path;
			/** the resulting node */
			nlohmann::json json;
			/** the list of all nodes wanted to be replaced by the content of this file */
			std::vector<nlohmann::json*> to_replaced_nodes;
		};

	public:

		nlohmann::json LoadJSONFile(FilePathParam const & path)
		{
			nlohmann::json result;
			ComputeSubstitutionChain(path);
			if (entries.size() > 0)
			{
				MakeSubstitutions();
				result = std::move(entries[0]->json);
				Clear();
			}
			return result;
		}

	protected:

		void ComputeSubstitutionChain(FilePathParam const & path)
		{
			LoaderEntry * entry = CreateEntry(path);
			if (entry == nullptr)
				return;
			stacked_entries.push_back(entry);
			DoComputeSubstitutionChain(entry, entry->json);
			stacked_entries.pop_back();
		}

		void DoComputeSubstitutionChain(LoaderEntry * entry, nlohmann::json & root)
		{
			assert(entry != nullptr);
			if (root.is_object() || root.is_array())
			{
				if (root.is_object())
				{
					nlohmann::json::iterator filename_it = root.find("@filename"); // replace any object that contains "@filename" = "..."
					if (filename_it != root.end())
					{
						if (filename_it->is_string())
						{
							FilePathParam replacement_path(
								filename_it->get<std::string>(), 
								entry->path);

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

		void Clear()
		{
			size_t count = entries.size();
			for (size_t i = 0; i < count; ++i)
				delete(entries[i]);
			entries.clear();
			stacked_entries.clear();
		}

		LoaderEntry * FindEntry(FilePathParam const & path)
		{
			boost::filesystem::path const & resolved_path = path.GetResolvedPath();
			for (size_t i = 0; i < entries.size(); ++i)
				if (entries[i]->path == resolved_path)
					return entries[i];
			return nullptr;
		}

		LoaderEntry * FindOrCreateEntry(FilePathParam const & path, bool & infinite_recursion)
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

		LoaderEntry * CreateEntry(FilePathParam const & path)
		{
			nlohmann::json new_json = JSONTools::LoadJSONFile(path, false);
			if (new_json.empty())
				return nullptr;
			LoaderEntry * new_entry = new LoaderEntry();
			if (new_entry == nullptr)
				return nullptr;
			new_entry->json = std::move(new_json);
			new_entry->path = path.GetResolvedPath();
			entries.push_back(new_entry);
			return new_entry;
		}

		void MakeSubstitutions()
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


	protected:

		/** all the entries implied in the recursive loading */
		std::vector<LoaderEntry*> entries;
		/** a value to detect for infinite recursion */
		std::vector<LoaderEntry*> stacked_entries;
	};

	nlohmann::json JSONTools::LoadJSONFile(FilePathParam const & path, bool recursive)
	{
		if (!recursive)
		{
			Buffer<char> buffer = FileTools::LoadFile(path, true);
			if (buffer != nullptr)
				return JSONTools::Parse(buffer.data);
			return nlohmann::json(); // loading failure
		}
		else
		{
			JSONRecursiveLoader loader;
			return loader.LoadJSONFile(path);
		}
	}
  bool JSONTools::ShowConfigFile(nlohmann::json const & json, char const * filename)
  {
    if (filename == nullptr)
      return false;

    boost::filesystem::path result_dir;
    if (FileTools::CreateTemporaryDirectory("MyTempDirectory_%d", result_dir))
    {
      boost::filesystem::path path = result_dir / filename;

      std::ofstream stream(path.string().c_str());
      stream << json.dump(4);

      WinTools::ShowFile(result_dir);
      return true;
    }
    return false;
  }


}; // namespace chaos
