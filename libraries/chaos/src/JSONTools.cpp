#include <chaos/JSONTools.h>
#include <chaos/FileTools.h>

namespace chaos
{
 
  nlohmann::json resultx;

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



  void DuplicateJSON(nlohmann::json const & src, nlohmann::json & dst)
  {
    for (auto it : dst)
    {
      


    }


  }


  void FixJSON(nlohmann::json & j)
  {
    if (j.is_array() || j.is_structured() || j.is_object())
    {
      auto filename_iterator = j.find("@filename");

      if (filename_iterator != j.end())
      {
   //     j["@filename"] = nlohmann::json::array();
        *filename_iterator = nlohmann::json::array();

    //    j.value.object;


        //nlohmann::json::object j.va

        int k = 0;
        ++k;

      }

      for (auto it : j)
        FixJSON(it);

    //  j["XXX"] = nlohmann::json::array();
      //j.push_back();

    }
  }

  nlohmann::json JSONTools::LoadJSONFileRecursive(FilePathParam const & path)
  {
    

    Buffer<char> buffer = FileTools::LoadFile(path, true);
    if (buffer != nullptr)
    {
      // nlohmann::json 
     resultx = JSONTools::Parse(buffer.data);
     nlohmann::json res = nlohmann::json::array();
     DuplicateJSON(resultx, res);
     
     FixJSON(resultx);

      std::string d = resultx.dump();

      char const * tt = d.c_str();

     // result.begin()->



      for (auto & it : resultx)
      {
       


        std::string ss;
        JSONTools::GetAttribute(it, "@filename", ss);

        ss = ss;

      }
      size_t count = resultx.size();
      for (size_t i = 0; i < resultx.size(); ++i)
      {


        
        //nlohmann::json child = result[i];


        i = i;
      }
    }





    if (buffer != nullptr)
      return JSONTools::Parse(buffer.data);
    return nlohmann::json();

//    std::vector<std::pair<boost::filesystem::path, nlohmann::json>> json_objects;

//    nlohmann::json root = Parse(FileTools::LoadFile(path, true));



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
