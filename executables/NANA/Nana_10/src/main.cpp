#include <chaos/Chaos.h>

using namespace nana;




using namespace nana;

void myfunc(label *lb)
{
  boost::this_thread::sleep(boost::posix_time::milliseconds(3000));

  lb->caption("coucou");
}





int main(int argc, char ** argv, char ** env)
{













  return 0;


  form fm; // (nana::API::make_center(400, 150), appear::decorate<appear::taskbar>());
  label lb{ fm };
  lb.caption("Hello, world!");
  lb.bgcolor(colors::azure);

  place layout(fm);
  //The div-text
  layout.div("vert<><<><here weight=400><here2>><>");
  layout["here"] << lb;
  layout.collocate();



  boost::thread t{ myfunc , &lb };

  fm.show();
  exec();

  t.join();

  return 0;
}


