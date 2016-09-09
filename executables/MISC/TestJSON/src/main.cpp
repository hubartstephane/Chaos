
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>
#include <chaos/LogTools.h>

// extract tests from : https://github.com/nlohmann/json

void Test1(boost::filesystem::path const & p)
{
  chaos::Buffer<char> buf = chaos::FileTools::LoadFile(p, true);
  if (buf != nullptr)
  {


  }
}

void Test2()
{
  nlohmann::json j;

  j["pi"] = 3.141;

  j["happy"] = true;

  j["name"] = "Niels";

  j["nothing"] = nullptr;

  j["answer"]["everything"] = 42;

  j["list"] = { 1, 0, 2 };

  j["object"] = { { "currency", "USD" },{ "value", 42.99 } };

  std::string s = j.dump();
  chaos::LogTools::Log("%s", s.c_str());
}


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  boost::filesystem::path const & p = chaos::Application::GetInstance()->GetResourcesPath();

  Test1(p / "test.json");
  Test2();




  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


