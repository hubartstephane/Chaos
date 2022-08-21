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

using namespace nana;

void myfunc(int id)
{
  form fm(nana::API::make_center(400, 150), appear::decorate<appear::taskbar>());
  //It's unnecessary to specify a rectangle if useing
  //layout management.
  label lb{ fm };
  lb.caption("Hello, world!");
  //Set a background color, just for observation.
  lb.bgcolor(colors::azure);
  //Define a layout object for the form.
  place layout(fm);
  //The div-text
  layout.div("vert<><<><vertical here weight=80><here2>><>");

  button btn(fm, rectangle{ 20, 20, 150, 300 });

  btn.tooltip("truc");
  btn.caption(L"Quit");
  btn.events().click([](nana::arg_click const& arg) {});
  btn.events().click(API::exit);

  layout["here"] << lb << btn;

  int delta = 200 * id;

  auto p = fm.pos();
  fm.move(p.x + delta, p.y + delta);

  layout.collocate();
  fm.show();
  exec();


}

int main(int argc, char ** argv, char ** env)
{
  // XXX : PeekMessage get a message for a window that belongs to current thread
  //       That's why API::exit on one thread does not kill the other
  //       Depending on the order, one windows seems to be locked

  boost::thread t1{ myfunc , 0};
  boost::thread t2{ myfunc , 1};
  t1.join();
  t2.join();




  return 0;
}


