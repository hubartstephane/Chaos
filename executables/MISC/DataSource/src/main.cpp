#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/ReferencedObject.h>
#include <chaos/FileManager.h>



class DataSource : public chaos::ReferencedObject
{

protected:

};

class FileDataSource : public DataSource
{

protected:

  boost::filesystem::path path;
};

class OBJ : public chaos::ReferencedObject
{
public:

  DataSource * GetDataSource() { return data_source.get(); }

  DataSource const * GetDataSource() const { return data_source.get(); }

protected:

  chaos::shared_ptr<DataSource> data_source;
};












class OBJManager : public chaos::ReferencedObject
{
public:

  OBJ * LoadObj(char const * filename);

  OBJ const * FindObjByName(char const * name) const;

  OBJ const * FindObjByDataSource(char const * source_name) const;

protected:

  std::vector<chaos::shared_ptr<OBJ>> objects;
};


OBJ * OBJManager::LoadObj(char const * filename)
{

  return nullptr;
}

OBJ const * OBJManager::FindObjByName(char const * name) const
{
  if (name == nullptr)
    return nullptr;

  size_t count = objects.size();
  for (size_t i = 0; i < count; ++i)
  {
    OBJ const * obj = objects[i].get();
    if (obj == nullptr)
      continue;
    DataSource const * data_source = obj->GetDataSource();
    if (data_source == nullptr)
      continue;




  }
  return nullptr;
}

OBJ const * OBJManager::FindObjByDataSource(char const * source_name) const
{


  return nullptr;
}



class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
    DataSource * ds = new DataSource;

    auto * x = dynamic_cast<FileDataSource*>(ds);
 


    boost::filesystem::path p1 = "toto";
    boost::filesystem::path p2 = "Toto";

    bool b1 = (p1 == p2);



    file_manager->AddDirectory(GetResourcesPath(), true);









		return true;
	}
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


