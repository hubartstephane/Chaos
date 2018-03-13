#include <chaos/FilePath.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

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

}; // namespace chaos
