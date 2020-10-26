#include <chaos/Chaos.h>

int CHAOS_MAIN(int argc, char ** argv, char ** env)
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


