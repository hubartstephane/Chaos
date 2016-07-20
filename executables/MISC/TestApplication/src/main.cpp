
#include <chaos/LuaTools.h>
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  boost::filesystem::path p1 = chaos::Application::GetInstance()->GetApplicationPath();
  boost::filesystem::path p2 = chaos::Application::GetInstance()->GetApplicationFilename();


  std::cout << "Application path     : " << p1 << std::endl;
  std::cout << "Application filename : " << p2 << std::endl;

  chaos::WinTools::PressToContinue();

  return 0;
}


