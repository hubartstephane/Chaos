#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>

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

    static size_t const INVALID_INDEX = std::numeric_limits<size_t>::max();

  public:

    nlohmann::json RecursiveLoad(FilePathParam const & path)
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
     
      DoComputeSubstitutionChain(entry, entry->json);
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

              LoaderEntry * new_entry = FindOrCreateEntry(replacement_path);
              if (new_entry != nullptr)
              {
                new_entry->to_replaced_nodes.push_back(&root);
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
      stack_states = 0;
    }

    size_t FindEntryIndex(FilePathParam const & path)
    {
      boost::filesystem::path const & resolved_path = path.GetResolvedPath();
      for (size_t i = 0; i < entries.size(); ++i)
        if (entries[i]->path == resolved_path)
          return i;
      return INVALID_INDEX;
    }

    LoaderEntry * FindEntry(FilePathParam const & path)
    {
      size_t index = FindEntryIndex(path);
      if (index == INVALID_INDEX)
        return nullptr;
      return entries[index];
    }

    LoaderEntry * FindOrCreateEntry(FilePathParam const & path)
    {
      LoaderEntry * result = FindEntry(path);
      if (result != nullptr)
        return result;
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
    /** a utility value to detect for infinite recursion */
    size_t stack_states = 0;
  };

	nlohmann::json JSONTools::LoadJSONFile(FilePathParam const & path, bool recursive)
	{
    if (!recursive)
    {
      Buffer<char> buffer = FileTools::LoadFile(path, true);
      if (buffer != nullptr)
        return JSONTools::Parse(buffer.data);
    }
    else
    {
      JSONRecursiveLoader loader;
      return loader.RecursiveLoad(path);
    }
	}
}; // namespace chaos
