#include <chaos/FilePath.h>

namespace chaos
{

	FilePath::FilePath(char const * filename, FilePath const * in_parent_path):
		path(filename), 
		parent_path(in_parent_path)
	{
	
	}

	FilePath::FilePath(std::string const & filename, FilePath const * in_parent_path):
		path(filename), 
		parent_path(in_parent_path)
	{
	
	}

	FilePath::FilePath(boost::filesystem::path const & filename, FilePath const * in_parent_path):
		path(filename), 
		parent_path(in_parent_path)
	{
	}

	bool FilePath::MatchPath(boost::filesystem::path const & other_path) const
	{
	
		return false;
	}

}; // namespace chaos
