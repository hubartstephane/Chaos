
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  boost::filesystem::path const & p1 = chaos::Application::GetInstance()->GetApplicationPath();
  boost::filesystem::path const & p2 = chaos::Application::GetInstance()->GetApplicationFilename();
  boost::filesystem::path const & p3 = chaos::Application::GetInstance()->GetUserLocalPath();
  boost::filesystem::path const & p4 = chaos::Application::GetInstance()->GetResourcesPath();


  std::cout << "Application path            : " << p1 << std::endl;
  std::cout << "Application filename        : " << p2 << std::endl;
  std::cout << "Application user local path : " << p3 << std::endl;


  boost::filesystem::create_directories(p3);


  chaos::WinTools::ShowFile(p3.string().c_str());
    

  boost::filesystem::path p = p3 / "myfile.txt";
  FILE * f = fopen(p.string().c_str(), "w");
  if (f != nullptr)
  {
    fprintf(f, "hello my world");

    fclose(f);
  }

  chaos::WinTools::PressToContinue();

  return 0;
}


