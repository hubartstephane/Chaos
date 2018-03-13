#pragma once

#include <chaos/StandardHeaders.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

	// XXX : this is a temporary object. It is used to give parameters to Load/Save functions and retrieve the resolved filename
  //       it internally use pointer on temp objet. Do not concerve it

	class FilePath
	{
	public:

    /** constructor */
    FilePath();
		/** constructor */
		FilePath(char const * in_basic_path, boost::filesystem::path const * in_reference_path = nullptr);
		/** constructor */
		FilePath(std::string const & in_string_path, boost::filesystem::path const * in_reference_path = nullptr);
		/** constructor */
		FilePath(boost::filesystem::path const & in_filesystem_path, boost::filesystem::path const * in_reference_path = nullptr);

    /** resolve if necessary and return the path */
    boost::filesystem::path const & GetResolvedPath() const;

	public:

		/** if path was given as a basic C string */
		char const * basic_path = nullptr;
		/** if path was given as a std::string */
		std::string const * string_path = nullptr;
		/** if path was given as a filesystem */
		boost::filesystem::path const * filesystem_path = nullptr;

    /** the reference path */
    boost::filesystem::path const * reference_path = nullptr;

  protected:

		/** the path after resolution */
		mutable boost::filesystem::path resolved_path;
	};

}; // namespace chaos
