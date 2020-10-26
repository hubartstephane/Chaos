
#include <chaos/Chaos.h>

// if a ConsoleApp is NOT run from a Console, it is given one !! 
// 

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{


  HWND consoleWnd = GetConsoleWindow();
  DWORD dwProcessId;
  GetWindowThreadProcessId(consoleWnd, &dwProcessId);
  if (GetCurrentProcessId() == dwProcessId)
  {
    std::cout << "I have my own console, press enter to exit" << std::endl;
  }
  else
  {
    std::cout << "This Console is not mine, good bye" << std::endl;
  }





  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  _write(1, "test write(1, ...)\n", 19);
  _write(2, "test write(2, ...)\n", 19);

  std::cout << "test   std::cout << string" << std::endl;
  std::cerr << "test   std::cerr << string" << std::endl;

  printf("test printf(...)\n");

  fprintf(stdout, "test fprintf(...) to stdout\n");
  fprintf(stderr, "test fprintf(...) to stderr\n");

  int i;
  std::cin >> i;

  return 0;
}


