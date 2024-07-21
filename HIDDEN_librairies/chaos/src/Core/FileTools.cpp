#include "chaos/ChaosPCH.h"
#include "chaos/ChaosInternals.h"

namespace chaos
{
	CHAOS_IMPLEMENT_ENUM_BITMASK_METHOD(LoadFileFlag, nullptr, CHAOS_API);

	namespace FileTools
	{
		namespace GlobalVariables
		{
#if _DEBUG
			CHAOS_GLOBAL_VARIABLE(bool, ShowLoadedFile, false);
			CHAOS_GLOBAL_VARIABLE(bool, NoDirectResourceFiles, false);
#endif
		};

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

		static std::vector<std::string> DoReadFileLines(boost::filesystem::path const& path)
		{
			std::vector<std::string> result;

			std::ifstream file(path.string().c_str());
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

		static Buffer<char> DoLoadFile(boost::filesystem::path const& resolved_path, LoadFileFlag flags)
		{
			Buffer<char> result;

			// load the content
			std::ifstream file(resolved_path.string().c_str(), std::ifstream::binary);
			if (file)
			{
				std::streampos start = file.tellg();
				file.seekg(0, std::ios::end);
				std::streampos end = file.tellg();
				file.seekg(0, std::ios::beg);

				size_t file_size = (size_t)(end - start);

				bool ascii = ((flags & LoadFileFlag::ASCII) == LoadFileFlag::ASCII);

				result = SharedBufferPolicy<char>::NewBuffer(file_size + ((ascii) ? 1 : 0));

				if (result != nullptr)
				{
					file.read((char*)result.data, file_size);
					if (file.gcount() != file_size) // read all or failure
						result = Buffer<char>();
					else if (ascii)
						result.data[file_size] = 0;
				}
			}
			return result;
		}

#if _DEBUG // File Redirection

		static boost::filesystem::path BuildRedirectedPath(boost::filesystem::path const& p, boost::filesystem::path const & build_path, boost::filesystem::path const& src_path)
		{
			// work with absolute path
			if (p.is_relative())
			{
				return BuildRedirectedPath(boost::filesystem::current_path() / p, build_path, src_path);
			}
			// search whether incomming path is inside the build_path
			auto it1 = p.begin();
			auto it2 = build_path.begin();

			while (it1 != p.end() && it2 != build_path.end())
			{
				if (*it1 != *it2)
					return {};
				++it1;
				++it2;
			}
			if (it2 != build_path.end()) // has all directories of build_path been consummed ?
				return {};

			// make substitution, build_path prefix to src_path prefix
			boost::filesystem::path relative_path = p.lexically_relative(build_path);
			boost::filesystem::path result = (src_path / relative_path);
			return result.lexically_normal();
		}
#endif // _DEBUG


		bool WithFile(FilePathParam const& path, LightweightFunction<bool(boost::filesystem::path const& p)> func)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();

			// File Redirection
#if _DEBUG

			if (Application const* application = Application::GetConstInstance())
			{
				if (!GlobalVariables::NoDirectResourceFiles.Get())
				{
					boost::filesystem::path const& build_path = application->GetRedirectionBuildPath();
					if (!build_path.empty())
					{
						std::vector<boost::filesystem::path> const& direct_access_paths = application->GetRedirectionSourcePaths();
						for (boost::filesystem::path const& direct_access_path : direct_access_paths)
						{
							boost::filesystem::path redirected_path = BuildRedirectedPath(resolved_path, build_path, direct_access_path);
							if (!redirected_path.empty())
								if (boost::filesystem::exists(redirected_path))
									if (func(redirected_path))
										return true;
						}
					}
				}
			}
#endif // _DEBUG

			// normal access
			if (boost::filesystem::exists(resolved_path))
				if (func(resolved_path))
					return true;
			// failure
			return false;
		}

		bool WithDirectoryContent(FilePathParam const& p, LightweightFunction<bool(boost::filesystem::path const& p)> func)
		{
			std::vector<boost::filesystem::path> processed_relative_paths;

			auto process_path_func = [&processed_relative_paths, func](boost::filesystem::path const& p)
			{
				if (boost::filesystem::is_directory(p))
				{
					for (auto it = boost::filesystem::directory_iterator(p) ; it != boost::filesystem::directory_iterator(); ++it)
					{
#if _DEBUG // File Redirection

						boost::filesystem::path relative_path = it->path().lexically_relative(p);
						if (std::find(processed_relative_paths.begin(), processed_relative_paths.end(), relative_path) != processed_relative_paths.end())
							continue;
						processed_relative_paths.push_back(relative_path);
#endif
						if (func(it->path()))
							return true;
					}
				}
				return false;
			};

			return WithFile(p, process_path_func);
		}

		boost::filesystem::path GetRedirectedPath(boost::filesystem::path const& p)
		{
			boost::filesystem::path result;
			WithFile(p, [&result](boost::filesystem::path const & p)
			{
				result = p;
				return true; // stops
			});
			return result;
		}

		bool IsTypedFile(FilePathParam const& path, char const* expected_ext)
		{
			boost::filesystem::path const& resolved_path = path.GetResolvedPath();
			return DoIsTypedFile(resolved_path.string().c_str(), expected_ext); // use an utility function because path to string give a volatile object
		}

		Buffer<char> LoadFile(FilePathParam const& path, LoadFileFlag flags)
		{
			Buffer<char> result;
			WithFile(path, [&result, &path, flags](boost::filesystem::path const&p)
			{
				result = DoLoadFile(p, flags);
#if _DEBUG
				if (result && GlobalVariables::ShowLoadedFile.Get())
				{
					Log::Message("LoadFile [%s] -> [%s]    size = [%d]", path.GetResolvedPath().string().c_str(), p.string().c_str(), result.bufsize);
				}
#endif
				return result; // convert to bool
			});

			if (result == nullptr && int(flags & LoadFileFlag::NO_ERROR_TRACE) == 0)
			{
				Log::Error("LoadFile fails [%s]", path.GetResolvedPath().string().c_str());
			}
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

		std::vector<std::string> ReadFileLines(FilePathParam const& path, LoadFileFlag flags)
		{
			std::vector<std::string> result;
			WithFile(path, [&result, &path](boost::filesystem::path const&p)
			{
				result = DoReadFileLines(p);
#if _DEBUG
				if ((result.size() > 0) && GlobalVariables::ShowLoadedFile.Get())
				{
					Log::Message("ReadFileLines [%s] -> [%s]    line count = [%d]", path.GetResolvedPath().string().c_str(), p.string().c_str(), result.size());
				}
#endif
				return (result.size() > 0); // convert to bool
			});

			if ((result.size() == 0) && int(flags & LoadFileFlag::NO_ERROR_TRACE) == 0)
			{
				Log::Error("ReadFileLines fails [%s]", path.GetResolvedPath().string().c_str());
			}

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