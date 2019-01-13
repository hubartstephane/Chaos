#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/StringTools.h>
#include <chaos/AllocatorTools.h>
#include <chaos/Application.h>

namespace chaos
{

	bool FileTools::DoIsTypedFile(char const * filename, char const * expected_ext)
	{
		assert(filename != nullptr);
		assert(expected_ext != nullptr);
		// find the extension
		char const * extension = strchr(filename, '.');
		if (extension == nullptr)
			return false;

		while (extension != nullptr)
		{
			char const * next_extension = strchr(extension + 1, '.');
			if (next_extension == nullptr)
				break;
			extension = next_extension;
		}
		if (expected_ext[0] == '.') // do not include separator
			++expected_ext;
		if (extension[0] == '.') // do not include separator
			++extension;

		// compare the extension
		return (_stricmp(expected_ext, extension) == 0);
	}

	bool FileTools::IsTypedFile(FilePathParam const & path, char const * expected_ext)
	{
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();		
		return DoIsTypedFile(resolved_path.string().c_str(), expected_ext); // use an utility function because path to string give a volatile object
	}

	boost::filesystem::path const & FileTools::GetRedirectedPath(boost::filesystem::path const & src, boost::filesystem::path & tmp)
	{
#if !_DEBUG
		return src;
#else

		Application const * application = Application::GetConstInstance();
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

	Buffer<char> FileTools::LoadFile(FilePathParam const & path, bool ascii)
	{
		Buffer<char> result;

		// resolve the path
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		// redirect path to resources for direct access
		boost::filesystem::path tmp;
		boost::filesystem::path const & redirected_path = GetRedirectedPath(resolved_path, tmp);

		// load the content
		std::ifstream file(redirected_path.string().c_str(), std::ifstream::binary);
		if (file)
		{
			std::streampos start = file.tellg();
			file.seekg(0, std::ios::end);
			std::streampos end = file.tellg();
			file.seekg(0, std::ios::beg);

			size_t file_size = (size_t)(end - start);

			result = SharedBufferPolicy<char>::NewBuffer(file_size + (ascii ? 1 : 0));

			if (result != nullptr)
			{
				file.read((char *)result.data, file_size);
				if (file.gcount() != file_size) // read all or failure
					result = Buffer<char>();
				else if (ascii)
					result.data[file_size] = 0;
			}
		}
		return result;
	}
	bool FileTools::CreateTemporaryDirectory(char const * pattern, boost::filesystem::path & result)
	{
		boost::filesystem::path temp_path = boost::filesystem::temp_directory_path();

		boost::filesystem::path uniq_path = (pattern == nullptr) ?
			boost::filesystem::unique_path(temp_path / "%%%%-%%%%-%%%%-%%%%") :
			boost::filesystem::unique_path(temp_path / StringTools::Printf("%s_%%%%-%%%%-%%%%-%%%%", pattern));

		if (boost::filesystem::create_directories(uniq_path))
		{
			result = std::move(uniq_path);
			return true;
		}
		return false;
	}

	std::vector<std::string> FileTools::ReadFileLines(FilePathParam const & path)
	{
		std::vector<std::string> result;

		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		std::ifstream file(resolved_path.string().c_str());
		if (file)
		{
			std::string str;
			while (std::getline(file, str))
				result.push_back(std::move(str));

#if 0
			// read the lines
			std::copy(std::istream_iterator<std::string>(file),
				std::istream_iterator<std::string>(),
				std::back_inserter(result));
#endif
		}
		return result;
	}

	bool FileTools::WriteFileLines(FilePathParam const & path, std::vector<std::string> const & lines)
	{
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

		std::ofstream file(resolved_path.string().c_str());
		if (file)
		{
			// write the lines
			for (std::string const & str : lines)
				file << str << std::endl;
			return true;
		}
		return false;
	}

}; // namespace chaos