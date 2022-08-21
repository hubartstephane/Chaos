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
  form fm;
  drawing{ fm }.draw([](paint::graphics& graph)
  {
    std::string hw = "Hello, world!";
    auto hw_size = graph.text_extent_size(hw);
    graph.string(
      point{ static_cast<int>(graph.width() - hw_size.width) / 2,
      static_cast<int>(graph.height() - hw_size.height) / 2 }
    , hw);
  });
  fm.show();
  exec();

  return 0;
}


