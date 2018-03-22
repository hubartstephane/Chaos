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





  static nlohmann::json * LoadAndMakeRecursiveSubstitution(
    FilePathParam const & path,
    std::vector<std::pair<boost::filesystem::path, nlohmann::json*>> & json_map,
    std::vector<size_t> & used_json)
  {
    size_t const INVALID_INDEX = std::numeric_limits<size_t>::max();

    boost::filesystem::path const & resolved_path = path.GetResolvedPath();

    // search whether the wanted path is already in the maps
    // detect infinite loop
    size_t path_index = INVALID_INDEX;
    for (size_t i = 0; (i < json_map.size()) && (path_index == INVALID_INDEX) ; ++i)
    {
      if (json_map[i].first == resolved_path) // path already handled
      {
        if (std::find(used_json.begin(), used_json.end(), i) != used_json.end()) // infinite recursion detected
          return nullptr;
        path_index = i;
      }   
    }
    // do we have the node in cache ?
    if (path_index != INVALID_INDEX)
      return json_map[path_index].second;
    // no => we need to make a recursive loading





    return nullptr;
  }

  // XXX : current_file_index, because json_map can be resized
  //
  static void MakeRecursiveSubstitution(
    nlohmann::json * root,
    size_t current_file_index,
    std::vector<std::pair<boost::filesystem::path, nlohmann::json*>> & json_map,
    std::vector<size_t> & used_json)
  {
    if (root == nullptr)
      return;

    if (root->is_object() || root->is_array())
    {
      if (root->is_object())
      {
        nlohmann::json::iterator filename_it = root->find("@filename");
        if (filename_it != root->end())
        {
          if (filename_it->is_string())
          {
            boost::filesystem::path const & current_path = json_map[current_file_index].first;

            nlohmann::json * substitution = LoadAndMakeRecursiveSubstitution(
              FilePathParam(filename_it->get<std::string>(), current_path),
              json_map,
              used_json
            );
            std::swap(*root, *substitution);
            return;
          }
        }
      }
      // recursive part for array and objects
      for (nlohmann::json::iterator it = root->begin(); it != root->end(); ++it)
        MakeRecursiveSubstitution(&*it, current_file_index, json_map, used_json);
    }
  }

	nlohmann::json JSONTools::LoadJSONFile(FilePathParam const & path, bool recursive)
	{
		nlohmann::json result;
		Buffer<char> buffer = FileTools::LoadFile(path, true);
		if (buffer != nullptr)
			result = JSONTools::Parse(buffer.data);
    
    if (0 && recursive)
    {
      std::vector<std::pair<boost::filesystem::path, nlohmann::json*>> json_map; // a mapping between a path and a already parsed JSON content
      std::vector<size_t> used_json; // due to recursive nature of the substition, we use a 'stack' to detect/avoid infinite recursion

      // make recursive substitution
      size_t current_file_index = json_map.size();
      json_map.push_back(std::make_pair(path.GetResolvedPath(), &result));
      used_json.push_back(current_file_index);
      MakeRecursiveSubstitution(&result, current_file_index, json_map, used_json);
      // delete temporary JSON objects
      for (size_t i = 1; i < json_map.size(); ++i) // we start at 1, because the very first element is allocated on the STACK, not the HEAP
        delete(json_map[i].second);
    }

    return result;
	}
}; // namespace chaos
