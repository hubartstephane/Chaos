#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>

using namespace nana;



int _tmain(int argc, char ** argv, char ** env)
{
 
  bool has_decoration = true;
  bool taskbar = true; 
  bool floating = false; 
  bool no_activate = false; 
  bool min = true; 
  bool max = true;
  bool sizable = true;
  appearance app = appearance(has_decoration, taskbar, floating, no_activate, min, max, sizable);

  form fm(nana::API::make_center(400, 150), app);
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
  btn.events().click([&fm](nana::arg_click const& arg){
  
    nana::msgbox::button_t buttons = msgbox::button_t::yes_no_cancel;

    nana::msgbox m(fm, "msgbox example", buttons);
    m << "This is a msgbox example.";
    m.icon(m.icon_question);
    m();
  });
  //btn.events().click(API::exit);


  layout["here"] << lb << btn;
  layout.collocate();
  fm.show();
  exec();

  return 0;
}


