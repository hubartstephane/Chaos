#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::WinTools::AllocConsoleAndRedirectStdOutput();



  const char * literal_string = R"BEGIN(
hello tout le monde
essai 1
truc
)BEGIN";

  std::cout << literal_string << std::endl;
  std::cout << "======================" << std::endl;

  chaos::WinTools::PressToContinue();

  return 0;
}


