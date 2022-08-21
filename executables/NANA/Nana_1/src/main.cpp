#include "chaos/Chaos.h"

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

  form fm(nana::API::make_center(400, 150),
                          appear::decorate<appear::taskbar>());
  //It's unnecessary to specify a rectangle if useing
  //layout management.
  label lb{ fm };
  lb.caption("Hello, world!");
  //Set a background color, just for observation.
  lb.bgcolor(colors::azure);
  //Define a layout object for the form.
  place layout(fm);
  //The div-text
  layout.div("vert<><<><here weight=80><here2>><>");
  layout["here"] << lb;
  layout.collocate();
  fm.show();
  exec();

  return 0;
}


