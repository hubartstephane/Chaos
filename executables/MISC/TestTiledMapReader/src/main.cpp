#include <chaos/WinTools.h>
#include <chaos/Application.h>
#include <chaos/StringTools.h>
#include <chaos/WinTools.h>
#include <chaos/MathTools.h>
#include <chaos/LogTools.h>
#include <chaos/SparseWriteBuffer.h>
#include <chaos/Buffer.h>
#include <chaos/TiledMapReader.h>
#include <chaos/FileTools.h>

void TestFromFile()
{
  chaos::Application * application = chaos::Application::GetInstance();
  if (application == nullptr)
    return;

  boost::filesystem::path const & resource_path = application->GetResourcesPath();

  chaos::Buffer<char> buffer = chaos::FileTools::LoadFile(resource_path / "tiled_map.tmx", false);
  if (buffer != nullptr)
  {

  }
}

int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize<chaos::Application>(argc, argv, env);

	chaos::WinTools::AllocConsoleAndRedirectStdOutput();
	


  TestFromFile();

	chaos::WinTools::PressToContinue();

	return 0;
}


