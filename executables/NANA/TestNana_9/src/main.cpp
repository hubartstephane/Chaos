#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>

using namespace nana;


#if 0
struct T
{
  std::string a;
  double b;
};


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{


  std::vector<T> Ts;
  //Prepare data
  for (auto i = 0; i < 10; ++i)
  {
    std::string n = chaos::StringTools::Printf("hello%d", 100 - i);
    Ts.push_back(T{ n, 12.0 + (float)i });
  }



  form fm;




  listbox list{ fm, rectangle{ 10, 40, 400, 400 } };
  list.append_header("A");
  list.append_header("B");

  

  list.checkable(true);

  list.enable_single(true, true);


  list.events().selected([&fm](const arg_listbox & cat) {
  
    int i = 0;
    ++i;
  
  });
 
  list.events().click.connect_front([&fm](const arg_click & arg) {
  
    arg.stop_propagation();
  });

  button btn{ fm, rectangle{ 10, 10, 100, 20 } };
  btn.caption("Load");
  btn.events().click([&list, &Ts]
  {
    auto cat = list.at(0);

    list.auto_draw(false);
    for (auto & t : Ts)
    {
      auto pos = cat.size();
      //Insert a new item, then set the text of the second column.
      cat.push_back(t.a);
      cat.at(pos).text(1, std::to_string(t.b));
    }
    list.auto_draw(true);
  });
  fm.show();
  exec();

  return 0;
}



#else


void on_menu_item(nana::menu::item_proxy& ip)
{
  std::size_t index = ip.index(); //Get the index of the clicking item.
  std::cout << "Menu item index:  " << index << std::endl;
}


int CHAOS_MAIN(int argc, char ** argv, char ** env)
{

  using namespace nana;
  form fm;
  fm.caption(L"Popup the menu when right clicking the button");
  ;//We need a menu object
  menu mobj;
  mobj.append("Item 0", &on_menu_item);
  mobj.append_splitter();
  mobj.append("Item 1", &on_menu_item);
  ;//Now we need a button.
  button btn(fm, nana::rectangle(nana::point(10, 10), nana::size(100, 25)));
  btn.caption("Popup Menu");
  ;//Popup the menu when right clicking the button.
   //btn.events().click(menu_popuper(mobj)) ;
  ;//Or popuping the menu with a specified coordinate when any mouse button is clicked.
  btn.events().mouse_down(menu_popuper(mobj, btn, nana::point(100, 36), mouse::any_button));


  fm.events().mouse_up([&fm, &mobj, & btn](arg_mouse const & arg) {
  
    // XXX : i tried different versions
    //
    //         1/ if (arg.right_button)   => right_button always false => not working
    //
    //         2/ remove if(...) but use   menu_popuper(... mouse::right_button)  => not working
    if (arg.button == mouse::right_button)
      menu_popuper(mobj, btn, arg.pos, mouse::any_button)(arg);
  });


  fm.show();
  exec();

}


#endif

