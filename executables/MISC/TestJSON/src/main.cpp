
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
        file << j << std::endl;
        


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

void Test3(boost::filesystem::path const & filename, boost::filesystem::path const & dst_dir)
{
  chaos::Buffer<char> buf = chaos::FileTools::LoadFile(filename, true);
  if (buf != nullptr)
  {
    std::ofstream file;

    file.open((dst_dir / "Test3.txt").string().c_str(), std::ofstream::out);
    if (file)
    {
      try
      {
        nlohmann::json::parser_callback_t cb = [&file](int depth, nlohmann::json::parse_event_t event, nlohmann::json & parsed) -> bool
        {
          file << "depth [" << depth << "]    name [" << parsed << "]    event [";

          /// the parser read `{` and started to process a JSON object
          if (event == nlohmann::json::parse_event_t::object_start)
            file << "object_start]";
          /// the parser read `}` and finished processing a JSON object            
          if (event == nlohmann::json::parse_event_t::object_end)
            file << "object_end]";
            /// the parser read `[` and started to process a JSON array              
          if (event == nlohmann::json::parse_event_t::array_start)          
            file << "array_start]";
          /// the parser read `]` and finished processing a JSON array          
          if (event == nlohmann::json::parse_event_t::array_end)                  
            file << "array_end]";            
          /// the parser read a key of a value in an object                  
          if (event == nlohmann::json::parse_event_t::key)                    
            file << "key]";            
          /// the parser finished reading a JSON value                                      
          if (event == nlohmann::json::parse_event_t::value)                      
            file << "value]";
            
          
          if (parsed == nlohmann::json("currency"))
              file << "   CURRENCY FOUND !!!";
 
          file << std::endl;

          return true;
        };

        nlohmann::json j = nlohmann::json::parse(buf.data, cb);
      }
      catch (std::exception & e)
      {
        char const * error = e.what();
        file << "EXCEPTION : " << e.what() << std::endl;
      }
    }
  }
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
    Test3(p / "test.json", dst_p);

    chaos::WinTools::ShowFile(dst_p.string().c_str());
  }

  chaos::WinTools::PressToContinue();

  chaos::Application::Finalize();

  return 0;
}


