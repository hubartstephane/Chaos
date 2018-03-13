#include <chaos/FilePath.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

	FilePath::FilePath()
	{
	}

	FilePath::FilePath(char const * in_path, boost::filesystem::path const * in_reference_path):
		path(in_path), 
		reference_path(in_reference_path)
	{
	
	}

	FilePath::FilePath(std::string const & in_path, boost::filesystem::path const * in_reference_path):
		path(in_path), 
		reference_path(in_reference_path)
	{
	
	}

	FilePath::FilePath(boost::filesystem::path const & in_path, boost::filesystem::path const * in_reference_path):
		path(in_path), 
		reference_path(in_reference_path)
	{
	}

	// ==============================================================
	// FILE PATH PARAM
	// ==============================================================

	FilePathParam::FilePathParam(char const * in_basic_path)
		:basic_path(in_basic_path)
	{
	}

	FilePathParam::FilePathParam(std::string const & in_string_path)
		:string_path(&in_string_path)
	{
	}

	FilePathParam::FilePathParam(boost::filesystem::path const & in_filesystem_path)
		:filesystem_path(&in_filesystem_path)
	{
	}

	FilePathParam::FilePathParam(FilePath & in_file_path)
		:file_path(&in_file_path)
	{
	}

}; // namespace chaos
