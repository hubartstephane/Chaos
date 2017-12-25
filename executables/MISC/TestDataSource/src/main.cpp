#include <chaos/SimpleWin32Class.h>
#include <chaos/SimpleWin32Window.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>
#include <chaos/FileTools.h>
#include <chaos/ReferencedObject.h>





class DataSource : public chaos::ReferencedObject
{

protected:

};

class FileDataSource : public DataSource
{

protected:

  boost::filesystem::path path;
};




class FileManager : public chaos::ReferencedObject
{



};




class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
    DataSource * ds = new DataSource;

    auto * x = dynamic_cast<FileDataSource*>(ds);
 


		return true;
	}
};

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


