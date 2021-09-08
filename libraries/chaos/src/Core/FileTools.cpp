#include <chaos/Chaos.h>

namespace chaos
{
	namespace FileTools
	{
		static bool DoIsTypedFile(char const* filename, char const* expected_ext)
		{
			assert(filename != nullptr);
			assert(expected_ext != nullptr);
			// find the extension
			char const* extension = strchr(filename, '.');
			if (extension == nullptr)
				return false;

			while (extension != nullptr)
			{
				char const* next_extension = strchr(extension + 1, '.');
				if (next_extension == nullptr)
					break;
				extension = next_extension;
			}
			if (expected_ext[0] == '.') // do not include separator
				++expected_ext;
			if (extension[0] == '.') // do not include separator
				++extension;

			// compare the extension
			return (StringTools::Stricmp(expected_ext, extension) == 0);
		}

		static std::vector<std::string> DoReadFileLines(boost::filesystem::path const& path, bool* success_open)
		{
			assert(success_open != nullptr && *success_open == false); // caller responsability

			std::vector<std::string> result;

			std::ifstream file(path.string().c_str());
			if (file)
			{
				*success_open = true;

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

		CHAOS_HELP_TEXT(CMD, "-ShowLoadedFile");

		static Buffer<char> DoLoadFile(boost::filesystem::path const& resolved_path, bool ascii, bool* success_open)
		{
			assert(success_open != nullptr && *success_open == false); // caller responsability

			Buffer<char> result;

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

				result = SharedBufferPolicy<char>::NewBuffer(file_size + (ascii ? 1 : 0));

				if (result != nullptr)
				{
					file.read((char*)result.data, file_size);
					if (file.gcount() != file_size) // read all or failure
						result = Buffer<char>();
					else if (ascii)
						result.data[file_size] = 0;
				}
			}

#if _DEBUG
			if (Application::HasApplicationCommandLineFlag("-ShowLoadedFile")) // CMDLINE
			{
				Log::Message("LoadFile [%s]    size = [%d]", resolved_path.string().c_str(), result.bufsize);
			}
#endif
			return result;
		}

		CHAOS_HELP_TEXT(CMD, "-NoDirectResourceFiles")

		bool ForEachRedirectedPath(FilePathParam const& path, std::function<bool(boost::filesystem::path const& p)> func)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();

#if _DEBUG // we cannot use 'CHAOS_CAN_REDIRECT_RESOURCE_FILES' inside libraries !!!
			if (Application const* application = Application::GetConstInstance())
			{
				if (!application->HasCommandLineFlag("-NoDirectResourceFiles")) // CMDLINE
				{
					boost::filesystem::path redirected_path;
					if (GetRedirectedPath(resolved_path, redirected_path))
					{
						if (func(redirected_path))
							return true;
					}
				}
			}
#endif
			return func(resolved_path);
		}

#if _DEBUG // we cannot use 'CHAOS_CAN_REDIRECT_RESOURCE_FILES' inside libraries !!!









		bool GetRedirectedPath(boost::filesystem::path const& p, boost::filesystem::path& redirected_path)
		{
			Application const* application = Application::GetConstInstance();
			if (application == nullptr)
				return false;
			// try to get the source and build directories
			boost::filesystem::path const& src_path = application->GetRedirectionSourcePath();
			if (src_path.empty())
				return false;
			boost::filesystem::path const& build_path = application->GetRedirectionBuildPath();
			if (build_path.empty())
				return false;

			// search whether incomming path is inside the build_path
			auto it1 = p.begin();
			auto it2 = build_path.begin();

			while (it1 != p.end() && it2 != build_path.end())
			{
				auto u = *it1;
				auto v = *it2;


				if (*it1 != *it2)
					return false;
				++it1;
				++it2;
			}
			if (it2 != build_path.end()) // has all directories of build_path been consummed ?
				return false;

			// make substitution, build_path prefix to src_path prefix

			auto a = src_path;
			auto b = build_path;
			auto c = p;
			auto d = p.lexically_relative(build_path);



			redirected_path = (src_path / p.lexically_relative(build_path));
			redirected_path.normalize();
			return true;
		}
#endif // _DEBUG


		bool ForEachRedirectedDirectoryContent(FilePathParam const& path, std::function<bool(boost::filesystem::path const& p)> func)
		{
			boost::filesystem::directory_iterator it;
			ForEachRedirectedPath(path, [&it](boost::filesystem::path const& p)
			{
				try
				{
					if (boost::filesystem::is_directory(p))
					{
						it = boost::filesystem::directory_iterator(p);
						if (it != boost::filesystem::directory_iterator())
							return true;
					}
				}
				catch (...)
				{
				}
				return false;
			});

			// iterate until functor returns true
			while (it != boost::filesystem::directory_iterator())
			{
				if (func(it->path()))
					return true;
				++it;
			}
			return false;
		}




		boost::filesystem::directory_iterator GetDirectoryIterator(FilePathParam const& path)
		{
			boost::filesystem::directory_iterator result;
			ForEachRedirectedPath(path, [&result](boost::filesystem::path const& p)
			{
				try
				{
					if (boost::filesystem::is_directory(p))
					{
						result = boost::filesystem::directory_iterator(p);
						if (result != boost::filesystem::directory_iterator())
							return true;
					}
				}
				catch (...)
				{
				}
				return false;
			});
			return result;
		}

		bool IsTypedFile(FilePathParam const& path, char const* expected_ext)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();
			return DoIsTypedFile(resolved_path.string().c_str(), expected_ext); // use an utility function because path to string give a volatile object
		}

		Buffer<char> LoadFile(FilePathParam const& path, bool ascii, bool* success_open)
		{
			// use a temporary open result, if not provided
			bool default_success_open = false;
			if (success_open == nullptr)
				success_open = &default_success_open;
			*success_open = false;

			Buffer<char> result;
			ForEachRedirectedPath(path, [&result, ascii, &success_open](boost::filesystem::path const&p) 
			{
				result = DoLoadFile(p, ascii, success_open);
				return *success_open;
			});
			return result;
		}

		bool CreateTemporaryDirectory(char const* pattern, boost::filesystem::path& result)
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

		std::vector<std::string> ReadFileLines(FilePathParam const& path, bool* success_open)
		{
			// use a temporary open result, if not provided
			bool default_success_open = false;
			if (success_open == nullptr)
				success_open = &default_success_open;
			*success_open = false;

			std::vector<std::string> result;
			ForEachRedirectedPath(path, [&result, &success_open](boost::filesystem::path const&p) 
			{
				result = DoReadFileLines(p, success_open);
				return *success_open;
			});
			return result;
		}



		bool WriteFileLines(FilePathParam const& path, std::vector<std::string> const& lines)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();

			std::ofstream file(resolved_path.string().c_str());
			if (file)
			{
				// write the lines
				for (std::string const& str : lines)
					file << str << '\n';
				return true;
			}
			return false;
		}

		boost::filesystem::path GetUniquePath(FilePathParam const& path, char const* format, bool create_empty_file, int max_iterations)
		{
			boost::filesystem::path dir_path = path.GetResolvedPath();

			int i = 0;
			while (max_iterations != 0)
			{
				// max iterations reached ?
				if (max_iterations > 0)
					--max_iterations;

				// compute the full path
				boost::filesystem::path file_path = dir_path / StringTools::Printf(format, i);

				if (!create_empty_file)
				{
					// path does not exits => returns
					if (!boost::filesystem::exists(file_path))
						return file_path;
				}
				else
				{
					// try to create the file (check if not already existing)
					HANDLE hFile = CreateFileA(
						file_path.string().c_str(),
						GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE,
						NULL,
						CREATE_NEW,
						FILE_ATTRIBUTE_NORMAL,
						NULL);
					// success ?
					if (hFile != INVALID_HANDLE_VALUE)
					{
						CloseHandle(hFile);
						return file_path;
					}
					// the only error supported is when the file already exist (an access error could produce an infinite loop)
					if (GetLastError() != ERROR_FILE_EXISTS)
						break;
				}
				++i;
			}
			return boost::filesystem::path();
		}

	}; // namespace FileTools

}; // namespace chaos