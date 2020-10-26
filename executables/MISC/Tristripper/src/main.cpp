#include <chaos/Chaos.h>

class MyApplication : public chaos::Application
{
protected:

  virtual bool Main() override
  {

    return true;
  }
};

int CHAOS_MAIN(int argc, char ** argv, char ** env)
{
  chaos::RunApplication<MyApplication>(argc, argv, env);

  return 0;
}


