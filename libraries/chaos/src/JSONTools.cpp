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

	static nlohmann::json MakeRecursiveJSON(FilePathParam const & path, nlohmann::json result, std::vector<std::pair<boost::filesystem::path, nlohmann::json>> & json_map, std::vector<size_t> & used_json)
	{
		size_t const INVALID_INDEX = std::numeric_limits<size_t>::max();

		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		// search whether the wanted path is already in the map
		// detect infinite loop
		size_t path_index = INVALID_INDEX;
		for (size_t i = 0 ; i < json_map.size() ; ++i)
		{
			if (json_map[i].first == resolved_path) // path already handled
			{
				if (std::find(used_json.begin(), used_json.end(), i) != used_json.end()) // infinite recursion detected
					return nlohmann::json();
			}		
			path_index = i;
		}
		// 

		if (result.is_array())




		json_map.push_back(std::make_pair(resolved_path, result));
		used_json.push_back(json_map.size() - 1);

		used_json.pop_back();
		return result;
	}

	nlohmann::json JSONTools::LoadJSONFile(FilePathParam const & path, bool recursive)
	{

		nlohmann::json result;

		Buffer<char> buffer = FileTools::LoadFile(path, true);
		if (buffer != nullptr)
			result = JSONTools::Parse(buffer.data);

		if (recursive)
		{
			std::vector<std::pair<boost::filesystem::path, nlohmann::json>> json_map; // a mapping between a path and a already parsed JSON content
			std::vector<size_t> used_json; // due to recursive nature of the substition, we use a 'stack' to detect/avoid infinite recursion
		
			result = MakeRecursiveJSON(path, result, json_map, used_json);
		}
		return result;
	}
}; // namespace chaos
