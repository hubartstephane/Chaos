#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

class MyApplication : public chaos::Application
{
protected:

  virtual bool Main() override
  {
	  auto [i, j] = { 3, 4 };
  
  
  
  
  
  
  
  

    return true;
  }
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::RunApplication<MyApplication>(argc, argv, env);
 
  return 0;
}


