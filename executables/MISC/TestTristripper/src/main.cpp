#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::RunApplication<MyApplication>(argc, argv, env);

  return 0;
}


