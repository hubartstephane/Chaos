#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH PARAM
	// ==============================================================

	// XXX : An object that is used to convert common objects (C string, C++ string, boost path) into a boost path object
  //       (apply a reference path if needed)

	class FilePathParam
	{
	public:

    /** constructor */
    FilePathParam(char const * basic_path);
    /** constructor */
    FilePathParam(std::string const & string_path);
    /** constructor */
    FilePathParam(boost::filesystem::path const & filesystem_path);
		/** constructor */
		FilePathParam(char const * basic_path, boost::filesystem::path const & reference_path);
		/** constructor */
		FilePathParam(std::string const & string_path, boost::filesystem::path const & reference_path);
		/** constructor */
		FilePathParam(boost::filesystem::path const & filesystem_path, boost::filesystem::path const & reference_path);

    /** resolve if necessary and return the path */
    boost::filesystem::path const & GetResolvedPath() const;

  protected:

    /** internal method to compute the resolved path */
    void ResolvePath(char const * basic_path, std::string const * string_path, boost::filesystem::path const * filesystem_path, boost::filesystem::path const * reference_path);

  protected:

		/** the path after resolution */
		boost::filesystem::path resolved_path;
	};

}; // namespace chaos
