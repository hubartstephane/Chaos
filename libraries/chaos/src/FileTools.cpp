#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/StringTools.h>
#include <chaos/AllocatorTools.h>

namespace chaos
{

	char const * FileTools::GetFilenameExtension(FilePath const & path)
	{
		return nullptr;
#if 0

		char const * result = strchr(filename, '.');
		while (result != nullptr)
		{
			char const * next_result = strchr(result + 1, '.');
			if (next_result == nullptr)
				break;
			result = next_result;
		}
		if (result != nullptr)
		{
			if (result[0] == '.') // do not include separator
				++result;
			if (result[0] == 0) // empty extension is considered has no extension
				result = nullptr;
		}
		return result;
#endif
	}

	bool FileTools::IsTypedFile(FilePath const & path, char const * expected_ext)
	{
		assert(expected_ext != nullptr);
		return (_stricmp(expected_ext, GetFilenameExtension(path)) == 0);
	}

	boost::filesystem::path FileTools::ResolvePath(FilePath const & path)
	{
		if (path.resolved_path.empty())
		{
		
		}

		boost::filesystem::path result;

		if (path.basic_path != nullptr)
			result = path.basic_path;
		else if (path.string_path != nullptr)
			result = path.string_path->c_str();
		else if (path.filesystem_path != nullptr)
			result = *path.filesystem_path;

		return result;
	}


	Buffer<char> FileTools::LoadFile(FilePath const & path, bool ascii)
	{
		Buffer<char> result;

		// resolve the path
		boost::filesystem::path resolved_path = ResolvePath(path);

		// load the content
		std::ifstream file(resolved_path.string().c_str(), std::ifstream::binary);
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

			// in case of success try to give the path back to callers
			if (path.file_path != nullptr)
				path.file_path->resolved_path = std::move(resolved_path);
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

	std::vector<std::string> FileTools::ReadFileLines(FilePath const & path)
	{
		std::vector<std::string> result;

		boost::filesystem::path resolved_path = ResolvePath(path);

		std::ifstream file(resolved_path.string().c_str());
		if (file)
		{
			// read the lines
			std::copy(std::istream_iterator<std::string>(file),
				std::istream_iterator<std::string>(),
				std::back_inserter(result));
			// in case of success try to give the path back to callers
			if (path.file_path != nullptr)
				path.file_path->resolved_path = std::move(resolved_path);
		}
		return result;
	}

	bool FileTools::WriteFileLines(FilePath const & path, std::vector<std::string> const & lines)
	{
		boost::filesystem::path resolved_path = ResolvePath(path);

		std::ofstream file(resolved_path.string().c_str());
		if (file)
		{
			// write the lines
			for (std::string const & str : lines)
				file << str << std::endl;
			// in case of success try to give the path back to callers
			if (path.file_path != nullptr)
				path.file_path->resolved_path = std::move(resolved_path);
			return true;
		}
		return false;
	}

}; // namespace chaos