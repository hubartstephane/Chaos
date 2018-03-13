#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

	// XXX : this is a temporary object. It is used to give parameters to Load/Save functions and retrieve the resolved filename

	class FilePath
	{
	public:

		/** constructor */
		FilePath();
		/** constructor */
		FilePath(char const * in_path, boost::filesystem::path const * in_reference_path = nullptr);
		/** constructor */
		FilePath(std::string const & in_path, boost::filesystem::path const * in_reference_path = nullptr);
		/** constructor */
		FilePath(boost::filesystem::path const & in_path, boost::filesystem::path const * in_reference_path = nullptr);

	public:

		/** the path */
		boost::filesystem::path path;
		/** parent reference */
		boost::filesystem::path resolved_path;
		/** parent reference */
		boost::filesystem::path const * reference_path = nullptr;
	};

	// ==============================================================
	// FILE PATH PARAM
	// ==============================================================

	//
	// XXX : this class does not deserve to be instanciated by the user. 
	//       It is only an utility class to make parameter conversions between function calls easier
	//
	class FilePathParam
	{
	public:

		/** constructor */
		FilePathParam(char const * in_basic_path);
		/** constructor */
		FilePathParam(std::string const & in_string_path);
		/** constructor */
		FilePathParam(boost::filesystem::path const & in_filesystem_path);
		/** constructor */
		FilePathParam(FilePath & in_file_path);

	public:

		/** if path was given as a basic C string */
		char const * basic_path = nullptr;
		/** if path was given as a std::string */
		std::string const * string_path = nullptr;
		/** if path was given as a filesystem */
		boost::filesystem::path const * filesystem_path = nullptr;
		/** if the request is more complex and requires some data back after loading */
		FilePath * file_path = nullptr;	
	};

}; // namespace chaos
