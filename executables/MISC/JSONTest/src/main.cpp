
#include "chaos/Chaos.h"

// extract tests from : https://github.com/nlohmann/json

void TestValue(std::ofstream & file, nlohmann::json j, char const * title)
{
  file << title << " : " << std::endl;

  file << "  is_null() "    << j.is_null() << std::endl;
  file << "  is_boolean() " << j.is_boolean() << std::endl;
  file << "  is_number() "  << j.is_number() << std::endl;
  file << "  is_object() "  << j.is_object() << std::endl;
  file << "  is_array() "   << j.is_array() << std::endl;
  file << "  is_string() "  << j.is_string() << std::endl;
  file << "  type() "       << j.type() << std::endl;
}

void Test1(boost::filesystem::path const & filename, boost::filesystem::path const & dst_dir)
{
  chaos::Buffer<char> buf = chaos::FileTools::LoadFile(filename, chaos::LoadFileFlag::ASCII);
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

        // read a bool
        bool v1 = j["happy"].get<bool>();

        file << "happy = " << v1 << std::endl;

        TestValue(file, v1, "v1");

        // 2 ways to read a value
        auto v2 = j["list"].get<std::vector<int>>();

        std::vector<int> v3 = j["list"];

        for (auto v : v2)
          file << "VERSION 1 : list = " << v << std::endl;
        for (auto v : v3)
          file << "VERSION 2 : list = " << v << std::endl;

        file << "size() "  << j.size()  << std::endl;
        file << "empty() " << j.empty() << std::endl;
        file << "type() "  << j.type()  << std::endl;

        TestValue(file, v2, "v2");

        // trigger an error by
        try
        {
          auto v4 = j["name"].get<bool>(); // trigger an exception because "name" is a string, not a bool !
        }
        catch (std::exception & e)
        {
          char const * error = e.what();
          file << "EXCEPTION : " << e.what() << std::endl;
        }
        // read a mixed map
        auto v5 = j["object"];

        TestValue(file, v5, "v5");

        auto v6 = v5["currency"];
        auto v7 = v5["value"];

        file << "currency = " << v6 << std::endl;
        TestValue(file, v6, "v6");
        file << "value    = " << v7 << std::endl;
        TestValue(file, v7, "v7");

        // use iterators
        for (nlohmann::json::iterator it = j.begin(); it != j.end(); ++it)
          file << "ITERATOR 1 : " << *it << '\n';

        // use iterators
        for (auto const & it : j)
          file << "ITERATOR 2 : " << it << '\n';

        // key + value iterators
        std::unordered_map<std::string, nlohmann::json> v8 = j;
        for (auto const & it : v8)
        {
          file << "ITERATOR 3 : " << std::endl;
          file << "  KEY   : " << it.first  << std::endl;
          file << "  VALUE : " << it.second << std::endl;
        }

        TestValue(file, v8, "v8");
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
  chaos::Buffer<char> buf = chaos::FileTools::LoadFile(filename, chaos::LoadFileFlag::ASCII);
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

class MyApplication : public chaos::Application
{
protected:

	virtual bool Main() override
	{
		boost::filesystem::path const & p = GetResourcesPath();

		boost::filesystem::path dst_p;
		if (chaos::FileTools::CreateTemporaryDirectory("TestJSON", dst_p))
		{
			Test1(p / "test.json", dst_p);
			Test2(dst_p);
			Test3(p / "test.json", dst_p);

			chaos::WinTools::ShowFile(dst_p);
		}

		chaos::WinTools::PressToContinue();

		return true;
	}
};


int main(int argc, char ** argv, char ** env)
{
  return chaos::RunApplication<MyApplication>(argc, argv, env);
}