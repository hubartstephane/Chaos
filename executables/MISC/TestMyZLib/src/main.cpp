#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MyZLib.h>


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();




  chaos::WinTools::PressToContinue();

  return 0;
}


