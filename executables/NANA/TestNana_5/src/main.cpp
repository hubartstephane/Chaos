#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>

int _tmain(int argc, char ** argv, char ** env)
{

  using namespace nana;
  
  form fm;

  fm.caption(L"Hello, World!");

  button btn(fm, rectangle{ 20, 20, 150, 300 });

  btn.tooltip("truc");
  btn.caption(L"Quit");
  btn.events().click(API::exit);

  fm.show();
      
  exec();

  return 0;
}


