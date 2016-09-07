#include <chaos/StandardHeaders.h>
#include <chaos/StringTools.h>
#include <chaos/Application.h>

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

  chaos::Application::Finalize();

  return 0;
}


