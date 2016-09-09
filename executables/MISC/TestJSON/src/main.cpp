
#include <chaos/WinTools.h>
#include <chaos/StringTools.h>
#include <chaos/FileTools.h>
#include <chaos/Application.h>
#include <chaos/LogTools.h>

// extract tests from : https://github.com/nlohmann/json

void Test1(boost::filesystem::path const & filename, boost::filesystem::path const & dst_dir)
{
  chaos::Buffer<char> buf = chaos::FileTools::LoadFile(filename, true);
  if (buf != nullptr)
  {
    std::ofstream file;

    file.open((dst_dir / "Test1.txt").string().c_str(), std::ofstream::out);
    if (file)
    {
      try
      {
        nlohmann::json j = nlohmann::json::parse(buf.data);

        


        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it) 
        {
          file << *it << '\n';
        }

        float p = j["pi"];


        p = p;
      }
      catch (std::exception & e)
      {
        char const * error = e.what();
        file << "EXCEPTION : " << e.what() << std::endl;
      }
    }   
  }
}

void Test2(boost::filesystem::path const & dst_dir)
{
  nlohmann::json j;

  j["pi"]      = 3.141;
  j["happy"]   = true;
  j["name"]    = "Niels";
  j["nothing"] = nullptr;  
  j["list"]    = { 1, 0, 2 };
  j["object"]  = { { "currency", "USD" },{ "value", 42.99 } };
  j["answer"]["everything"] = 42;

  std::ofstream file;

  file.open((dst_dir / "Test2.txt").string().c_str(), std::ofstream::out);
  if (file)
    file << j.dump(4); // 4 = indent
}


int _tmain(int argc, char ** argv, char ** env)
{
  chaos::Application::Initialize(argc, argv, env);

  chaos::WinTools::AllocConsoleAndRedirectStdOutput();

  boost::filesystem::path const & p = chaos::Application::GetInstance()->GetResourcesPath();

  boost::filesystem::path dst_p;
  if (chaos::FileTools::CreateTemporaryDirectory("TestJSON", dst_p))
  {
    Test1(p / "test.json", dst_p);
    Test2(dst_p);

    chaos::WinTools::ShowFile(dst_p.string().c_str());
  }

  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


