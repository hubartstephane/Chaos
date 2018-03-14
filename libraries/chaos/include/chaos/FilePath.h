#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

	// XXX : this is a temporary object. It is used to give parameters to Load/Save functions and retrieve the resolved filename
  //       it internally use pointer on temp objet. Do not concerve it

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
		FilePathParam(char const * in_basic_path, boost::filesystem::path const & in_reference_path);
		/** constructor */
		FilePathParam(std::string const & in_string_path, boost::filesystem::path const & in_reference_path);
		/** constructor */
		FilePathParam(boost::filesystem::path const & in_filesystem_path, boost::filesystem::path const & in_reference_path);

    /** resolve if necessary and return the path */
    boost::filesystem::path const & GetResolvedPath() const;

  protected:

		/** if path was given as a basic C string */
		char const * basic_path = nullptr;
		/** if path was given as a std::string */
		std::string const * string_path = nullptr;
		/** if path was given as a filesystem */
		boost::filesystem::path const * filesystem_path = nullptr;

    /** the reference path */
    boost::filesystem::path const * reference_path = nullptr;

		/** the path after resolution */
		mutable boost::filesystem::path resolved_path;
	};

}; // namespace chaos
