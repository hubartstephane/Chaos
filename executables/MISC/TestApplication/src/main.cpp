
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & p1 = GetApplicationPath();
		boost::filesystem::path const & p2 = GetApplicationFilename();
		boost::filesystem::path const & p3 = GetUserLocalPath();
		boost::filesystem::path const & p4 = GetResourcesPath();


		std::cout << "Application path            : " << p1 << std::endl;
		std::cout << "Application filename        : " << p2 << std::endl;
		std::cout << "Application user local path : " << p3 << std::endl;


		boost::filesystem::create_directories(p3);


		chaos::WinTools::ShowFile(p3);


		boost::filesystem::path p = p3 / "myfile.txt";
		FILE * f = fopen(p.string().c_str(), "w");
		if (f != nullptr)
		{
			fprintf(f, "hello my world");

			fclose(f);
		}

		chaos::WinTools::PressToContinue();

		return true;
	}
};

int _tmain(int argc, char ** argv, char ** env)
{
	chaos::RunApplication<MyApplication>(argc, argv, env);
	return 0;
}


