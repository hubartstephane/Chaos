#include <chaos/Chaos.h>

#include <nana/gui.hpp>
#include <nana/gui/widgets/menubar.hpp>
#include <nana/gui/widgets/textbox.hpp>
#include <nana/gui/place.hpp>
#include <nana/gui/msgbox.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/label.hpp>
//#include <nana/gui/widgets/frame.hpp>
//#include <nana/gui/wvl.hpp>

int main(int argc, char ** argv, char ** env)
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


