#include <chaos/FileTools.h>
#include <chaos/LogTools.h>
#include <chaos/StringTools.h>
#include <chaos/AllocatorTools.h>

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

	Buffer<char> FileTools::LoadFile(FilePathParam const & path, bool ascii)
	{
		Buffer<char> result;

		// resolve the path
		boost::filesystem::path const & resolved_path = path.GetResolvedPath();

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
			// read the lines
			std::copy(std::istream_iterator<std::string>(file),
				std::istream_iterator<std::string>(),
				std::back_inserter(result));
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