#include <chaos/FilePath.h>

namespace chaos
{

	// ==============================================================
	// FILE PATH
	// ==============================================================

  FilePath::FilePath()
  {

  }

	FilePath::FilePath(char const * in_basic_path, boost::filesystem::path const * in_reference_path):
    basic_path(in_basic_path),
		reference_path(in_reference_path)
	{
	
	}

	FilePath::FilePath(std::string const & in_string_path, boost::filesystem::path const * in_reference_path):
    string_path(&in_string_path),
		reference_path(in_reference_path)
	{
	
	}

	FilePath::FilePath(boost::filesystem::path const & in_filesystem_path, boost::filesystem::path const * in_reference_path):
    filesystem_path(&in_filesystem_path),
		reference_path(in_reference_path)
	{
	}

  boost::filesystem::path const & FilePath::GetResolvedPath() const
  {
    if (resolved_path.empty()) // compute the path or use cache result
    {

    }
    return resolved_path;
  }

#if 0
  boost::filesystem::path FileTools::ResolvePath(FilePath const & path)
  {
    if (path.resolved_path.empty())
    {

    }

    boost::filesystem::path result;

    if (path.basic_path != nullptr)
      result = path.basic_path;
    else if (path.string_path != nullptr)
      result = path.string_path->c_str();
    else if (path.filesystem_path != nullptr)
      result = *path.filesystem_path;

    return result;
  }
#endif





}; // namespace chaos
