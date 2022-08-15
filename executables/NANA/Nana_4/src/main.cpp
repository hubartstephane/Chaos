#include <chaos/Chaos.h>

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


