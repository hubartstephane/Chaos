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




  nlohmann::json JSONTools::LoadJSONFileRecursive(FilePathParam const & path)
  {
    std::vector<std::pair<boost::filesystem::path, nlohmann::json>> json_objects;

    nlohmann::json root = Parse(FileTools::LoadFile(path, true));


    return root;
  }

#if 0

  nlohmann::json JSONTools::Replace(nlohmann::json root)
  {
    for (size_t i = 0; i < root.size(); ++i)
    {
      nlohmann::json child = root[i];
      if (child.is_string())

    }


  }


    json_objects
   

    while (true)
    {
      Buffer<char> buffer = FileTools::LoadFile(json_objects)


    }



    json_objects.push_back(std::make_pair(path.GetResolvedPath(), ))



    return json_objects[0].second;
  }
#endif
}; // namespace chaos
