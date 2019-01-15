#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 




#include "Ludum43Game.h"

#include <chaos/Buffer.h> 
#include <chaos/Application.h> 
#include <chaos/FilePath.h> 


#define CHAOS_CAN_REDIRECT_RESOURCE_FILES (_DEBUG && defined CHAOS_PROJECT_PATH && defined CHAOS_PROJECT_SRC_PATH && defined CHAOS_PROJECT_BUILD_PATH)

#if CHAOS_CAN_REDIRECT_RESOURCE_FILES
bool GetRedirectedPath(boost::filesystem::path const & p, boost::filesystem::path & redirected_path)
{
	chaos::Application const * application = chaos::Application::GetConstInstance();
	//if (application == nullptr)
	//	return false;
	//	if (!application->HasCommandLineFlag("-DirectResourceFiles"))
	//		return false;


	static boost::filesystem::path src_path = CHAOS_PROJECT_SRC_PATH;
	static boost::filesystem::path build_path = CHAOS_PROJECT_BUILD_PATH;

	// search whether incomming path is inside the build_path
	auto it1 = p.begin();
	auto it2 = build_path.begin();

	while (it1 != p.end() && it2 != build_path.end())
	{
		if (*it1 != *it2)
			return false;
		++it1;
		++it2;
	}
	if (it2 != build_path.end()) // has all directories of build_path been consummed ?
		return false;
	
	// make substitution, build_path prefix to src_path prefix
	redirected_path = (src_path / p.lexically_relative(build_path));
	redirected_path.normalize();
	return true;
}
#endif // CHAOS_CAN_REDIRECT_RESOURCE_FILES



chaos::Buffer<char> DoLoadFile(boost::filesystem::path const & resolved_path, bool ascii, bool * success_open)
{
	assert(success_open != nullptr && *success_open == false); // caller responsability

	chaos::Buffer<char> result;

	// load the content
	std::ifstream file(resolved_path.string().c_str(), std::ifstream::binary);
	if (file)
	{
		*success_open = true;

		std::streampos start = file.tellg();
		file.seekg(0, std::ios::end);
		std::streampos end = file.tellg();
		file.seekg(0, std::ios::beg);

		size_t file_size = (size_t)(end - start);

		result = chaos::SharedBufferPolicy<char>::NewBuffer(file_size + (ascii ? 1 : 0));

		if (result != nullptr)
		{
			file.read((char *)result.data, file_size);
			if (file.gcount() != file_size) // read all or failure
				result = chaos::Buffer<char>();
			else if (ascii)
				result.data[file_size] = 0;
		}
	}
	return result;
}



chaos::Buffer<char> LoadFile(chaos::FilePathParam const & path, bool ascii, bool * success_open)
{
	// use a temporary open result, if not provided
	bool default_success_open = false;
	if (success_open == nullptr)
		success_open = &default_success_open;
	*success_open = false;

	chaos::Buffer<char> result;

	// resolve the path
	boost::filesystem::path const & resolved_path = path.GetResolvedPath();

	// try the alternative
#if CHAOS_CAN_REDIRECT_RESOURCE_FILES
	boost::filesystem::path redirected_path;
	if (GetRedirectedPath(resolved_path, redirected_path))
	{
		result = DoLoadFile(redirected_path, ascii, success_open);
		if (*success_open)
			return result;
	}
#endif // CHAOS_CAN_REDIRECT_RESOURCE_FILES 
	return DoLoadFile(resolved_path, ascii, success_open);
}

int _tmain(int argc, char ** argv, char ** env)
{



	bool open_success = false;
	chaos::Buffer<char> buffer = LoadFile("D:\\Personnel\\Programmation\\build\\vs2015\\executables\\LUDUMDARE\\Ludum43\\DEBUG\\x32\\resources\\config.json", true, &open_success);


	open_success = open_success;
	return 0;

	chaos::MyGLFW::SingleWindowApplicationParams params;
	params.monitor = nullptr;
	params.width = 500;
	params.height = 500;
	params.monitor_index = 0;
	death::RunGame<LudumGame>(argc, argv, env, params);
	return 0;
}


