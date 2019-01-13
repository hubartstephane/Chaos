#include <chaos/StandardHeaders.h> 
#include <death/Death.h> 




#include "Ludum43Game.h"

#include <chaos/Buffer.h> 
#include <chaos/Application.h> 
#include <chaos/FilePath.h> 


#define CHAOS_CAN_REDIRECT_RESOURCE_FILES (_DEBUG && defined CHAOS_PROJECT_PATH && defined CHAOS_PROJECT_SRC_PATH && defined CHAOS_PROJECT_BUILD_PATH)

#if CHAOS_CAN_REDIRECT_RESOURCE_FILES
bool GetRedirectedPath(boost::filesystem::path const & src, boost::filesystem::path & redirected_path)
{
	chaos::Application const * application = chaos::Application::GetConstInstance();
	//if (application == nullptr)
	//	return false;
//	if (!application->HasCommandLineFlag("-RedirectResources"))
//		return false;

	// SRC = "E:\\programmation\\build\\vs2015\\executables\\LUDUMDARE\\Ludum43\\DEBUG\\x32\\resources\\config.json"

	// CHAOS_PROJECT_PATH 
	// ------------------
	// "executables/LUDUMDARE/Ludum43"

	// CHAOS_PROJECT_SRC_PATH 
	// ----------------------
	// "E:/programmation/Code/executables/LUDUMDARE/Ludum43"

	// CHAOS_PROJECT_BUILD_PATH
	// ------------------------
	// "E:/programmation/build/vs2015/executables/LUDUMDARE/Ludum43"

	auto a = CHAOS_PROJECT_PATH;
	auto b = CHAOS_PROJECT_SRC_PATH;
	auto c = CHAOS_PROJECT_BUILD_PATH;








	return false;
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













#if 0

	boost::filesystem::path const & GetRedirectedPath(boost::filesystem::path const & src, boost::filesystem::path & tmp)
	{
#if !_DEBUG
		return src;
#else

		chaos::Application const * application = chaos::Application::GetConstInstance();
		if (application == nullptr)
			return src;

		if (!application->HasCommandLineFlag("-RedirectResources"))
			return src;


#if defined XXXX
		//	ddd
#endif

#if 0

		"E:\\programmation\\build\\vs2015\\executables\\LUDUMDARE\\Ludum43\\DEBUG\\x32\\resources"

			E:\programmation\Code\executables\LUDUMDARE\Ludum43\resources


			build\\vs2015->Code

			Debug\x32->Supprimer




#endif


			auto xxx = src.relative_path();

		//boost::filesystem::path::relative_path

		boost::filesystem::path const & application_path = application->GetApplicationPath();

		boost::filesystem::path const & resources_path = application->GetResourcesPath();

		auto it1 = src.begin();
		auto it2 = application_path.begin();

		while (it1 != src.end() && it2 != application_path.end())
		{


			++it1;
			++it2;
		}


		return src;

#endif
	}

#endif














int _tmain(int argc, char ** argv, char ** env)
{



	bool open_success = false;
	chaos::Buffer<char> buffer = LoadFile("E:\\programmation\\build\\vs2015\\executables\\LUDUMDARE\\Ludum43\\DEBUG\\x32\\resources\\config.json", true, &open_success);


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


